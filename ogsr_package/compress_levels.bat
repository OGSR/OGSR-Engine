DEL READY\gamedata.db_base_levels_1
DEL READY\gamedata.db_base_levels_2
READY\bin\xrCompress.exe "X:\ogsr_package\Unpacked\base_levels" -ltx "..\..\datapack.ltx" -max_size 1920 -store
MOVE /Y base_levels.pack_#0 READY\
MOVE /Y base_levels.pack_#1 READY\
REN READY\base_levels.pack_#0 gamedata.db_base_levels_1
REN READY\base_levels.pack_#1 gamedata.db_base_levels_2