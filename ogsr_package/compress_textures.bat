DEL READY\gamedata.db_base_textures_1
DEL READY\gamedata.db_base_textures_2
DEL READY\gamedata.db_base_textures_3
READY\bin\xrCompress.exe "X:\ogsr_package\Unpacked\base_textures" -ltx "..\..\datapack.ltx" -max_size 1920 -store
MOVE /Y base_textures.pack_#0 READY\
MOVE /Y base_textures.pack_#1 READY\
MOVE /Y base_textures.pack_#2 READY\
REN READY\base_textures.pack_#0 gamedata.db_base_textures_1
REN READY\base_textures.pack_#1 gamedata.db_base_textures_2
REN READY\base_textures.pack_#2 gamedata.db_base_textures_3