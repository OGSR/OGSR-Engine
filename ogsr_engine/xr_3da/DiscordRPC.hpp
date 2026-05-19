#pragma once

class ENGINE_API DiscordRPC final
{
    const char* current_level_name_translated{};
    const char* current_level_name{};
    const char* active_task_text{};
    int64_t start_time{};

public:
    void Init();
    void Destroy();
    void Update(const char* level_name_translated = nullptr, const char* level_name = nullptr);
    void Set_active_task_text(const char* txt)
    {
        active_task_text = txt;
        Update();
    }
};

extern ENGINE_API DiscordRPC Discord;
