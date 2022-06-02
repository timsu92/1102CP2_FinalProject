SHELL := /bin/bash # Use bash syntax
.PHONY: install judge help
.DEFAULT: help

ENV_LIST=$(shell basename -a -s .env env/*.env)
#REGISTRY=dev-registry.aralego.com
REGISTRY=220.135.77.53:8081
BUILD_NAME=backend/teambuying-server
BUILD_TAG=latest

help:
	@echo "make judge: run map judge"
	@echo "make clean: clean generate files"

clean:
	@rm -rf tmp
	@rm -rf *.log

judge:
	@mkdir tmp
	@cp judge/judge.py tmp
	@cp player/player.cpp tmp/A.cpp
	@cp bot/bot.cpp tmp/B.cpp
	@cp map/88map tmp
	@g++ tmp/A.cpp -o tmp/A
	@g++ tmp/B.cpp -o tmp/B
	@cd tmp && python3 judge.py
	@cp tmp/*.log .
	@rm -rf tmp

