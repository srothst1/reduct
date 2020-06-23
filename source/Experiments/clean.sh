#!/bin/bash

ssh seagull "screen -S clientnode -p 0 -X quit; exit;"
ssh pigeon "screen -S clientnode -p 0 -X quit; exit;"
ssh parrot "screen -S clientnode -p 0 -X quit; exit;"
ssh pelican "screen -S clientnode -p 0 -X quit; exit;"
ssh owl "screen -S datanode -p 0 -X quit; rm -rf /local/dfs; exit;"
ssh loon "screen -S datanode -p 0 -X quit; rm -rf /local/dfs; exit;"
ssh swan "screen -S datanode -p 0 -X quit; rm -rf /local/dfs; exit;"
ssh dodo "screen -S datanode -p 0 -X quit; rm -rf /local/dfs; exit;"
