#include "stdafx.h"

#include "xrstring.h"

XRCORE_API extern str_container* g_pStringContainer = NULL;

struct str_container_impl
{
    xrCriticalSection cs;
    static constexpr size_t buffer_size = 1024u * 256u;
    str_value* buffer[buffer_size];
    int num_docs;

    str_container_impl()
    {
        num_docs = 0;
        ZeroMemory(buffer, sizeof(buffer));
    }

    str_value* find(str_value* value, const char* str) const
    {
        str_value* candidate = buffer[value->dwCRC % buffer_size];
        while (candidate)
        {
            if (candidate->dwCRC == value->dwCRC && candidate->dwLength == value->dwLength && !memcmp(candidate->value, str, value->dwLength))
            {
                return candidate;
            }

            candidate = candidate->next;
        }

        return nullptr;
    }

    void insert(str_value* value)
    {
        str_value** element = &buffer[value->dwCRC % buffer_size];
        value->next = *element;
        *element = value;
    }

    void clean()
    {
        for (size_t i = 0; i < buffer_size; ++i)
        {
            str_value** current = &buffer[i];

            while (*current != nullptr)
            {
                str_value* value = *current;
                if (!value->dwReference)
                {
                    *current = value->next;
                    xr_free(value);
                }
                else
                {
                    current = &value->next;
                }
            }
        }
    }

    void verify() const
    {
        Msg("strings verify started");
        for (size_t i = 0; i < buffer_size; ++i)
        {
            const str_value* value = buffer[i];
            while (value)
            {
                const auto crc = crc32(value->value, value->dwLength);
                string32 crc_str;
                R_ASSERT(crc == value->dwCRC, "CorePanic: read-only memory corruption (shared_strings)", itoa(value->dwCRC, crc_str, 16));
                R_ASSERT(value->dwLength == xr_strlen(value->value), "CorePanic: read-only memory corruption (shared_strings, internal structures)", value->value);
                value = value->next;
            }
        }
        Msg("strings verify completed");
    }

    void dump(FILE* f) const
    {
        for (size_t i = 0; i < buffer_size; ++i)
        {
            str_value* value = buffer[i];
            while (value)
            {
                fprintf(f, "ref[%4u]-len[%3u]-crc[%8X] : %s\n", value->dwReference, value->dwLength, value->dwCRC, value->value);
                value = value->next;
            }
        }
    }

    void dump(IWriter* f) const
    {
        for (size_t i = 0; i < buffer_size; ++i)
        {
            str_value* value = buffer[i];
            string4096 temp;
            while (value)
            {
                xr_sprintf(temp, sizeof(temp), "ref[%4u]-len[%3u]-crc[%8X] : %s\n", value->dwReference, value->dwLength, value->dwCRC, value->value);
                f->w_string(temp);
                value = value->next;
            }
        }
    }

    ptrdiff_t stat_economy() const
    {
        ptrdiff_t counter = 0;
        for (size_t i = 0; i < buffer_size; ++i)
        {
            const str_value* value = buffer[i];
            while (value)
            {
                counter -= sizeof(str_value);
                counter += (value->dwReference - 1) * (value->dwLength + 1);
                value = value->next;
            }
        }

        return counter;
    }
};

str_container::str_container() : impl(xr_new<str_container_impl>()) {}

str_value* str_container::dock(pcstr value) const
{
    if (nullptr == value)
        return nullptr;

    impl->cs.Enter();

    str_value* result = nullptr;

    // calc len
    const auto s_len = xr_strlen(value);
    const auto s_len_with_zero = s_len + 1;
    VERIFY(sizeof(str_value) + s_len_with_zero < 4096);

    // setup find structure
    char header[sizeof(str_value)];
    str_value* sv = (str_value*)header;
    sv->dwReference = 0;
    sv->dwLength = static_cast<u32>(s_len);
    sv->dwCRC = crc32(value, s_len);

    // search
    result = impl->find(sv, value);

#ifdef DEBUG
    const bool is_leaked_string = !xr_strcmp(value, "enter leaked string here");
#endif // DEBUG

    // it may be the case, string is not found or has "non-exact" match
    if (nullptr == result
#ifdef DEBUG
        || is_leaked_string
#endif // DEBUG
    )
    {
        result = static_cast<str_value*>(xr_malloc(sizeof(str_value) + s_len_with_zero));

#ifdef DEBUG
        static int num_leaked_string = 0;
        if (is_leaked_string)
        {
            ++num_leaked_string;
            Msg("leaked_string: %d 0x%08x", num_leaked_string, result);
        }
#endif // DEBUG

        result->dwReference = 0;
        result->dwLength = sv->dwLength;
        result->dwCRC = sv->dwCRC;
        CopyMemory(result->value, value, s_len_with_zero);

        impl->insert(result);
    }
    impl->cs.Leave();

    return result;
}

void str_container::clean() const
{
    impl->cs.Enter();
    impl->clean();
    impl->cs.Leave();
}

void str_container::verify() const
{
    impl->cs.Enter();
    impl->verify();
    impl->cs.Leave();
}

void str_container::dump() const
{
    impl->cs.Enter();
    FILE* F = fopen("d:\\$str_dump$.txt", "w");
    impl->dump(F);
    fclose(F);
    impl->cs.Leave();
}

size_t str_container::stat_economy() const
{
    impl->cs.Enter();
    ptrdiff_t counter = 0;
    counter -= sizeof(*this);
    counter += impl->stat_economy();
    impl->cs.Leave();
    return size_t(counter);
}

str_container::~str_container()
{
    clean();
    // dump ();
    xr_delete(impl);
}
