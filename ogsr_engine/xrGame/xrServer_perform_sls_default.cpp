#include "stdafx.h"
#include "xrserver.h"

void xrServer::SLS_Default()
{
    ASSERT_FMT(game->custom_sls_default(), "game->custom_sls_default() != true");

    if (game->custom_sls_default())
    {
        game->sls_default();
    }
}
