SHELL := /bin/bash # Use bash syntax
.PHONY: install judge help
.DEFAULT: help

help:
	@echo "make judge: judge with bot 1"
	@echo "make clean: clean generate files"

clean:
	@rm -rf tmp
	@rm -rf *.log

judge:
	@mkdir tmp
	@cp judge/judge.py tmp
	@cp map/88map tmp
	@echo "player be A, bot be B"
	@cp player/player.cpp tmp/A.cpp
	@cp bot/bot.cpp tmp/B.cpp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python3 judge.py
	@cp tmp/move.log move_player_be_A.log
	@echo "bot be A, player be B"
	@cp player/player.cpp tmp/B.cpp
	@cp bot/bot.cpp tmp/A.cpp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python3 judge.py
	@cp tmp/move.log move_player_be_B.log
	@rm -rf tmp

