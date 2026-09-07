"""Exercise the public brightness command without physical LCD hardware."""
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class AmoledBrightnessTests(unittest.TestCase):
    def test_qspi_command_scaling_clamping_and_errors(self):
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)
            (path / 'esp_err.h').write_text('''#pragma once
typedef int esp_err_t;
#define ESP_ERR_INVALID_ARG 258
''')
            (path / 'esp_lcd_panel_io.h').write_text('''#pragma once
#include <stddef.h>
#include "esp_err.h"
typedef void *esp_lcd_panel_io_handle_t;
esp_err_t esp_lcd_panel_io_tx_param(esp_lcd_panel_io_handle_t, int, const void *, size_t);
''')
            (path / 'test.c').write_text('''#include <assert.h>
#include "waveshare_amoled.h"
static int device, calls, command, result;
static uint8_t value;
esp_err_t esp_lcd_panel_io_tx_param(esp_lcd_panel_io_handle_t io, int cmd, const void *data, size_t size)
{
    assert(io == &device);
    assert(size == 1);
    calls++;
    command = cmd;
    value = *(const uint8_t *)data;
    return result;
}
int main(void)
{
    assert(waveshare_amoled_set_brightness(NULL, 50) == ESP_ERR_INVALID_ARG);
    assert(calls == 0);
    const uint8_t percent[] = {0, 1, 50, 100, 255};
    const uint8_t expected[] = {0, 2, 127, 255, 255};
    for (size_t i = 0; i < sizeof(percent); i++) {
        assert(waveshare_amoled_set_brightness(&device, percent[i]) == 0);
        assert(command == 0x02005100);
        assert(value == expected[i]);
    }
    assert(calls == 5);
    result = 73;
    assert(waveshare_amoled_set_brightness(&device, 50) == 73);
    return 0;
}
''')
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-I', str(path), '-I', str(ROOT / 'include'),
                            str(path / 'test.c'), '-o', str(path / 'test')],
                           check=True, capture_output=True, text=True)
            subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
