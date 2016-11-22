#!/bin/sh
scp -P 33850 Client/Client.out prameets@pc11.utahddc.geniracks.net:/users/prameets/
scp -P 33850 NFV/NFV.out prameets@pc1.utahddc.geniracks.net:/users/prameets/

scp -P 33850 Server/NFVServer.out prameets@pc10.utahddc.geniracks.net:/users/prameets
scp -P 33850 Server/script.sh prameets@pc10.utahddc.geniracks.net:/users/prameets

scp -P 33850 Server/NFVServer.out prameets@pc5.utahddc.geniracks.net:/users/prameets
scp -P 33850 Server/script.sh prameets@pc5.utahddc.geniracks.net:/users/prameets

scp -P 33850 Server/NFVServer.out prameets@pc4.utahddc.geniracks.net:/users/prameets
scp -P 33850 Server/script.sh prameets@pc4.utahddc.geniracks.net:/users/prameets
