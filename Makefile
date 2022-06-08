SHELL := /bin/bash # Use bash syntax
.PHONY: clean test judge1 judge2 help
.DEFAULT: help

help:
	@echo "make judge1: judge with bot 1"
	@echo "make judge2: judge with bot 2"
	@echo "make test: test player with sample input"
	@echo "make clean: clean generate files"

clean:
	@rm -rf tmp
	@rm -rf *.log

test:
	@mkdir tmp
	@cp player/player.cpp tmp/player.cpp
	@g++ tmp/player.cpp -o tmp/player
	@echo "test player with sampleA.in"
	@timeout 1 tmp/player < sample/sampleA.in
	@echo "test player with sampleB.in"
	@timeout 1 tmp/player < sample/sampleB.in
	@echo "test pass"
	@rm -rf tmp

judge1:
	@mkdir tmp
	@cp judge/judge.py tmp
	@cp map/88map tmp
	@echo "player be A, bot be B"
	@cp player/player.cpp tmp/A.cpp
	@cp bot/bot1.cpp tmp/B.cpp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python judge.py
	@cp tmp/move.log move_player_be_A.log
	@rm tmp/*.log
	@echo "bot be A, player be B"
	@cp player/player.cpp tmp/B.cpp
	@cp bot/bot1.cpp tmp/A.cpp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python judge.py
	@cp tmp/move.log move_player_be_B.log
	@rm -rf tmp

judge2:
	@mkdir tmp
	@cp judge/judge.py tmp
	@cp map/88map tmp
	@echo "player be A, bot be B"
	@cp player/player.cpp tmp/A.cpp
	@cp bot/bot2.cpp tmp/B.cpp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python judge.py
	@cp tmp/move.log move_player_be_A.log
	@rm tmp/*.log
	@echo "bot be A, player be B"
	@cp player/player.cpp tmp/B.cpp
	@cp bot/bot2.cpp tmp/A.cpp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python judge.py
	@cp tmp/move.log move_player_be_B.log
	@rm -rf tmp
