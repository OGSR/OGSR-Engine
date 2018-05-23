
--Тестовая сборка мусора при выходе из меню
if level.present() then
	level.add_call(function() return true end, function() collectgarbage("collect") end)
end
