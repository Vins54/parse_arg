/*
** parse_long_opt.c for parse_arg in /home/leroy_v/Perso/parse_arg
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Aug 18 19:58:38 2014 vincent leroy
** Last update Tue Aug 19 14:15:02 2014 vincent leroy
*/

#include <stdlib.h>
#include <string.h>

#include "parse_long_opt.h"

static int _get_opts_idx_for_opt(char *long_opt, const opts options[])
{
    int i;

    for (i = 0; options[i].cb != NULL; ++i)
        if (options[i].long_opt != NULL && strcmp(options[i].long_opt, long_opt) == 0)
            return i;

    return -1;
}

static int _split_opt_and_arg(const char *long_opt, char **next_opt, char **result_opt, char **result_arg)
{
    int nb_arg_parsed = 1;
    char *equal_pos = strchr(long_opt, '=');

    if (equal_pos != NULL)
    {
        *result_opt = strndup(long_opt, equal_pos - long_opt);
        *result_arg = strdup(equal_pos + 1);
    }
    else
    {
        *result_opt = strdup(long_opt);
        *result_arg = NULL;

        if (next_opt[0] != NULL && next_opt[0][0] == '-')
        {
            if (next_opt[0][1] == '-' && next_opt[0][2] == '\0')
            {
                ++nb_arg_parsed;
                if (next_opt[1] != NULL)
                {
                    ++nb_arg_parsed;
                    *result_arg = strdup(next_opt[1]);
                }
            }
        }
        else if (next_opt[0] != NULL)
        {
            ++nb_arg_parsed;
            *result_arg = strdup(next_opt[0]);
        }
    }

    return nb_arg_parsed;
}

static int _call_cb(char *arg, const opts *opt, opt_error *error)
{
    int nb_arg_used = 1;

    switch (opt->has_arg)
    {
        case NO_ARG:
            if (!opt->cb(NULL, opt->user_data, false))
                return set_parse_error(error, -1, 0, CALLBACK_ERROR);
            break;
        case OPTIONAL_ARG:
            if (!opt->cb(arg, opt->user_data, false))
                return set_parse_error(error, -1, 0, CALLBACK_ERROR);
            nb_arg_used += arg != NULL ? 1 : 0;
            break;
        case REQUIRED_ARG:
            if (arg == NULL)
                return set_parse_error(error, -1, 0, ARG_NOT_PROVIDE);
            if (!opt->cb(arg, opt->user_data, false))
                return set_parse_error(error, -1, 0, CALLBACK_ERROR);
            ++nb_arg_used;
            break;
    }

    return nb_arg_used;
}

int parse_long_opt(const char *long_opt, char **next_opt, const opts options[], opt_error *error)
{
    char *real_opt;
    char *real_arg;
    int nb_arg_parsed;

    nb_arg_parsed = _split_opt_and_arg(long_opt, next_opt, &real_opt, &real_arg);
    if (nb_arg_parsed == -1)
        return -1;

    int idx = _get_opts_idx_for_opt(real_opt, options);
    if (idx == -1)
        nb_arg_parsed = set_parse_error(error, -1, 0, UNKNOWN_OPT);
    else if (_call_cb(real_arg, &options[idx], error) != -1)
    {
        if (options[idx].has_arg == NO_ARG && nb_arg_parsed > 1)
            --nb_arg_parsed;
    }
    else
        nb_arg_parsed = -1;

    free(real_opt);
    free(real_arg);

    return nb_arg_parsed;
}
