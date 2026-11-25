"""Local fallback for single-language review findings. No text leaves the runner.

Argos Translate 1.9 packages contain OPUS-MT models by Jörg Tiedemann and
Santhosh Thottingal (CC BY 4.0). The package README is kept with each model.
"""

import hashlib
import os
import re
import shutil
import tempfile
import zipfile
from functools import lru_cache
from pathlib import Path
from urllib.request import urlopen


MODEL_URL = 'https://data.argosopentech.com/argospm/v1/'
MODEL_HASHES = {
    ('zh', 'en'): '62e7af5a3a48b530e47b7b3e5c78c2de79073ecd815750d2bf3ab35b4a67da2d',
    ('en', 'zh'): '433e7c4f034d87fbe2353161e05f18646d7999452f801a4e1f0378522b9850ab',
}
# Preserve Markdown code, URLs, and common register / hardware / code identifiers.
LITERAL = re.compile(
    r'\[[^\]\n]+\]\(https?://[^)\s]+\)|`+[^`\n]+`+|https?://[^\s<>\)\]\u3000-\u303f\u4e00-\u9fff\uff00-\uffef]+'
    r'|(?<![A-Za-z0-9_])(?:0[xX][0-9a-fA-F]+|[A-Za-z_][A-Za-z0-9]*_[A-Za-z0-9_]+'
    r'|[A-Z]{2,}[A-Z0-9]*(?:[-.][A-Z0-9]+)*|[A-Za-z]+\d+[A-Za-z0-9]*)(?![A-Za-z0-9_])'
)
SENTENCE = re.compile(r'(?<=[。！？；])|(?<=[.!?])\s+(?=[A-Z0-9`])')


# The general-purpose model otherwise renders common board-review terms literally.
ZH_TERMS = {'校验和': 'checksum', '桩测试': 'stub test',
            '实机验证': 'on-hardware verification', '板卡配置': 'board configuration'}
ZH_LITERAL = re.compile(LITERAL.pattern + '|' + '|'.join(ZH_TERMS))


@lru_cache(maxsize=2)
def _model(source, target):
    import ctranslate2
    import sentencepiece

    digest = MODEL_HASHES[source, target]
    root = Path(os.environ.get('RUNNER_TEMP', tempfile.gettempdir())) / 'codex-review-translation'
    name = f'translate-{source}_{target}-1_9'
    directory = root / f'{name}-{digest[:12]}'
    if not directory.is_dir():
        root.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=root) as staging:
            archive = Path(staging) / 'model.zip'
            with urlopen(f'{MODEL_URL}{name}.argosmodel', timeout=60) as response, archive.open('wb') as stream:
                shutil.copyfileobj(response, stream)
            with archive.open('rb') as stream:
                if hashlib.file_digest(stream, 'sha256').hexdigest() != digest:
                    raise ValueError(f'Checksum mismatch for {name}')
            with zipfile.ZipFile(archive) as package:
                for member in ('README.md', 'sentencepiece.model', 'model/config.json',
                               'model/shared_vocabulary.json', 'model/model.bin'):
                    package.extract(f'{name}/{member}', staging)
            (Path(staging) / name).rename(directory)

    tokenizer = sentencepiece.SentencePieceProcessor(model_file=str(directory / 'sentencepiece.model'))
    engine = ctranslate2.Translator(str(directory / 'model'), device='cpu', compute_type='int8', intra_threads=2)
    return tokenizer, engine


def _infer(text, tokenizer, engine):
    tokens = tokenizer.encode(text, out_type=str)
    if not tokens:
        return text
    # Split very long source text explicitly; translate_batch otherwise truncates it.
    chunks = [tokens[i:i + 180] for i in range(0, len(tokens), 180)]
    results = engine.translate_batch(chunks, beam_size=4, replace_unknowns=True,
                                     max_decoding_length=512, length_penalty=1, no_repeat_ngram_size=3)
    return ' '.join(tokenizer.decode_pieces(r.hypotheses[0]).replace('▁', ' ').strip() for r in results)


def _translate_line(line, tokenizer, engine, source):
    # Mask identifiers before sentence splitting so periods in URLs remain intact.
    literals = []
    prefix = 'Q'
    while re.search(rf'{prefix}\s*\d+', line):
        prefix += 'Q'

    def mask(match):
        token = match.group()
        literals.append(ZH_TERMS.get(token, token) if source == 'zh' else token)
        return f'{prefix}{len(literals)-1:02d}'

    masked = (ZH_LITERAL if source == 'zh' else LITERAL).sub(mask, line)
    sentences = []
    marker = re.compile(rf'{prefix}\s*(\d+)')
    parts = []
    for sentence in SENTENCE.split(masked):
        # Chinese technical prose often joins several independent conditions into
        # a long sentence. Keep each clause to avoid dropping a condition.
        parts.extend(re.split(r'(?<=，)', sentence) if len(sentence) > 64 else [sentence])
    for sentence in parts:
        if not sentence.strip():
            continue
        translated = _infer(sentence, tokenizer, engine)
        if sorted(marker.findall(sentence)) == sorted(marker.findall(translated)):
            rendered = marker.sub(lambda m: literals[int(m[1])], translated)
        else:
            # Some MT outputs omit or change a placeholder. In that case, translate
            # the surrounding prose separately and copy all source literals exactly.
            fragments = marker.split(sentence)
            rendered = ' '.join(literals[int(word)] if i % 2 else _infer(word, tokenizer, engine)
                                for i, word in enumerate(fragments) if word.strip())
        sentences.append(rendered)
    return ' '.join(sentences)


def translate(text: str, source: str, target: str) -> str:
    if source == target or not text.strip():
        return text
    tokenizer, engine = _model(source, target)
    return '\n'.join(_translate_line(line, tokenizer, engine, source) if line.strip() else line for line in text.split('\n'))
