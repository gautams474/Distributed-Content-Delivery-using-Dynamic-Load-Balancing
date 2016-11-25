#!/bin/sh
echo
echo Client
scp -P 33850 Client/Client.out prameets@pc11.utahddc.geniracks.net:/users/prameets/

echo
echo NFV
scp -P 33850 NFV/NFV.out prameets@pc1.utahddc.geniracks.net:/users/prameets/

echo
echo Server1
scp -P 33850 Server/NFVServer.out prameets@pc10.utahddc.geniracks.net:/users/prameets
scp -P 33850 Server/script.sh prameets@pc10.utahddc.geniracks.net:/users/prameets
# scp -P 33850 Content/my_file.txt prameets@pc10.utahddc.geniracks.net:/users/prameets/Content
scp -P 33850 FileDivision/file_handler.out prameets@pc10.utahddc.geniracks.net:/users/prameets

echo
echo Server2
scp -P 33850 Server/NFVServer.out prameets@pc5.utahddc.geniracks.net:/users/prameets
scp -P 33850 Server/script.sh prameets@pc5.utahddc.geniracks.net:/users/prameets
scp -P 33850 Content/my_file.txt prameets@pc5.utahddc.geniracks.net:/users/prameets/Content
scp -P 33850 FileDivision/file_handler.out prameets@pc5.utahddc.geniracks.net:/users/prameets

echo
echo Server3
scp -P 33850 Server/NFVServer.out prameets@pc4.utahddc.geniracks.net:/users/prameets
scp -P 33850 Server/script.sh prameets@pc4.utahddc.geniracks.net:/users/prameets
scp -P 33850 Content/my_file.txt prameets@pc4.utahddc.geniracks.net:/users/prameets/Content
scp -P 33850 FileDivision/file_handler.out prameets@pc4.utahddc.geniracks.net:/users/prameets


# scp -P 33850 prameets@pc11.utahddc.geniracks.net:/users/prameets/Content/my_file.txt /home/prameet/ACN-Project/