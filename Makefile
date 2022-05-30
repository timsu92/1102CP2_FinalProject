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

judge:
	mkdir tmp
	cp judge/judge.py tmp
	cp A/A.cpp tmp
	cp B/B.cpp tmp
	cp map/88map tmp
	g++ tmp/A.cpp -o tmp/A
	g++ tmp/B.cpp -o tmp/B
	cd tmp && python3 judge.py
	cp tmp/move.log .
	rm -rf tmp

