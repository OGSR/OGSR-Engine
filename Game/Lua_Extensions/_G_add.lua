--Добавить в _G.script. В самый верх!
--==============================================================================================
log1("--LUA version: ["..(jit.version or _VERSION).."]")
--==============================================================================================
FFI = require('ffi') --Делать только здесь и только один раз!
--==============================================================================================
math.randomseed(os.time())
--=======================================[LuaXML]===============================================
--_luaxml.script должен быть в папке scripts.
prefetch("_luaxml")
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
-- Взято из аддона Naxac'a
local to_ptrn = {
	[' '] = '%s',
	['.'] = '%.',
	['%'] = '%%',
	['*'] = '%*',
	['-'] = '%-'
}
string.explode = function( div,    -- разделитель
                           str,    -- строка
                           clear ) -- обрезать ли пробелы по краям строк (false/true)
	local ret = {}
	for s in str:gmatch("([^"..(to_ptrn[div] or div).."]+)") do
		ret[#ret+1] = (clear and s:trim() or s)
	end
	return ret
end
--==============================================================================================
-- очистка строки от "лишних" символов (полезно при работе с FS)
string.clear = function(str) --KRodin: поправил функцию, теперь работает.
	return str:gsub( '[/:<>|%*%?"\\]*', "" )
end
--==============================================================================================
-- Форматирование текста по ширине
-- http://www.amk-team.ru/forum/topic/13216-sborochnyy-ceh/?do=findComment&comment=959286
string.width = function(str, width, return_table, indent, paragrapf)
    indent    = indent    or 0
    paragrapf = paragrapf or 0
    width     = (width or 80) - indent

    local i = string.rep(' ', indent)
    local t = {}
    local line = string.rep(' ', paragrapf)

    for exp, word in str:gmatch('(%s-(%S+))') do
        if line == '' and #t ~= 0 then
            exp = word
        end

        local l_len = #line
        local len = l_len + #exp
        
        if len > width then
            t[#t+1], line = line, word
        else
            line = line .. exp
            if l_len == width then
                t[#t+1], line = line, ''
            end
        end
    end

    t[#t+1] = line

    if return_table then
        return t
    else
        return i .. table.concat(t, '\n' .. i)
    end
end
--==============================================================================================
function table.copy(tbl) --Возвращает копию таблицы.
	local mt = getmetatable(tbl)
	local res = {}
	for k, v in pairs(tbl) do
		if type(k) == "table" then
			k = table.copy(k)
		end
		if type(v) == "table" then
			v = table.copy(v)
		end
		res[k] = v
	end
	setmetatable(res, mt)
	return res
end
--=================================[CUIScriptWnd Extension]=====================================
-- Вызов обычный, как и в AddCallback, но после последнего аргумента можно передавать множество дополнительных.
function CUIScriptWnd:Connect(name, event, func, ...)
	local args = {...}
	return self:AddCallback( name, event, function() func(unpack(args)) end )
end
--==============================================================================================
local tostring_all_tbl = {
    ["table"] 	   = function(table, ind) --Распечатка таблиц
                        local res = "{" --Начало
                        local end_res = ind and "\n"..ind.."}" or "\n}" --Конец
                        local def_ind = "  " --Отступ от начала строки
                        ind = ind and ind..def_ind or def_ind --Для каждой "таблицы внутри таблицы" отступ увеличивается в 2 раза
                        for k, v in pairs(table) do
                            res = res.."\n"..ind.."["..tostring_all(k, ind).."] = "..tostring_all(v, ind)..","
                        end
                        return res..end_res
                    end,
    ["string"]    = function(v) return "'"..v.."'" end, --Строки берём в кавычки. Это нужно, чтобы можно было отличить строку "123" от числа 123, "nil" от nil и тд...
    ["number"]    = function(v) return tostring(v) end,
    ["boolean"]   = function(v) return tostring(v) end,
    ["nil"]       = function(v) return tostring(v) end,
    ["function"]  = function() return "[[FUNCTION]]" end, --Function, Userdata и Thread распечатать нельзя, просто заменим на строки
    ["userdata"]  = function() return "[[USERDATA]]" end,
    ["thread"]    = function() return "[[THREAD]]" end,
}
--Конвертирование чего угодно в строку. Использовать только для вывода в лог!
function tostring_all(obj, ind) --ind - служебный аргумент для распечатки "таблиц в таблицах".
	return tostring_all_tbl[type(obj)](obj, ind)
end

--Функциональная обёртка для log1.
--Делает дополнительную работу по конвертации параметров, которые не воспринимаются родной функцией string.format.
--Параметры можно передавать ЛЮБОГО ТИПА! В строке формата они должны стоять как %s (потому, что будут сконвертированы в строки).
function log3(fmt, ...)
	local args = {...}
	for i=1, #args do
		args[i] = tostring_all(args[i])
	end
	local res, msg = pcall(string.format, fmt, unpack(args))
	if not res then --Если произошла ошибка
		msg = "!!log3 failed: "..msg.."\n"..debug.traceback()
	end
	log1(msg)
end
--==============================================================================================
