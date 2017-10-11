--Добавить в _G.script. В самый верх!
--==============================================================================================
log1("--LUA version: ["..(jit.version or _VERSION).."]")
--====================================[LuaFileSystem]===========================================
--lfs.dll должен быть в папке bin. Утащил из Prosectors Project.
xpcall(function() require("lfs") end, log1)
--=======================================[LuaXML]===============================================
--_luaxml.lua должен быть в папке scripts.
xpcall(function() dofile(getFS():update_path("$game_scripts$", "_luaxml.lua")) end, log1)
--=================================[Функции из xrLuaFix]========================================
--Cрезать первое "слово"
string.trim_w = function(s) return s:gsub("%s-(%S+)%s+", '', 1) end
--Срезать пробелы слева
string.trim_l = function(s) return s:gsub("^%s+", '') end
--Срезать пробелы справа
string.trim_r = function(s) return s:gsub("%s*$", '') end
--Срезать пробелы слева и справа
string.trim   = function(s) return s:match( "^%s*(.*%S)" ) or "" end
------------------------------------------------------------------------------------------------
--Возвращает кол-во элементов в таблице. Для массивов надо использовать оператор #.
table.size = function(t)
	local size = 0
	for _ in pairs(t) do size = size + 1 end
	return size
end
--Возвращает случайный элемент массива. Для обычных таблиц такое делать нет никакого смысла. В движке было сделано как-то странно.
table.random = function(t)
	return t[math.random(#t)]
end
--Возвращает массив с ключами таблицы
table.keys = function(t)
	local arr = {}
	for k, _ in pairs(t) do
		table.insert(arr, k)
	end
	return arr
end
--Возвращает массив с значениями таблицы
table.values = function(t)
	local arr = {}
	for _, v in pairs(t) do
		table.insert(arr, v)
	end
	return arr
end
--==============================================================================================
