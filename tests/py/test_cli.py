import pytest

import kstool
from kstool.__main__ import main


def test_version_flag_prints_version(capsys):
    with pytest.raises(SystemExit) as exit_info:
        main(["--version"])
    assert exit_info.value.code == 0
    assert capsys.readouterr().out.strip() == f"kstool {kstool.__version__}"


def test_no_command_prints_help_and_returns_2(capsys):
    assert main([]) == 2
    assert "usage: kstool" in capsys.readouterr().out
