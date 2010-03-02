/* author Kevin Smith <ksmith@basho.com>
   copyright 2009-2010 Basho Technologies

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <string.h>

#include <erl_nif.h>

#include "emonk.h"
#include "emonk_util.h"

ErlNifResourceType* EMONK_CTX;

static int
emonk_load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    const char* name = "emonk.Context";
    int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
    EMONK_CTX = enif_open_resource_type(env, name, stop_vm, flags, NULL);
    if(EMONK_CTX == NULL) return -1;
    
    if(!JS_CStringsAreUTF8()) JS_SetCStringsAreUTF8();
    return 0;
}

static ERL_NIF_TERM
emonk_new_ctx(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    emonk_vm_t* vm = NULL;
    ERL_NIF_TERM ret;

    vm = (emonk_vm_t*) enif_alloc_resource(env, EMONK_CTX, sizeof(emonk_vm_t));
    ret = enif_make_resource(env, vm);
    enif_release_resource(env, vm);

    if(vm == NULL) return emonk_no_memory(env);
    vm->env = env;
    vm->error = 0;

    if(!init_vm(vm, env, argc, argv)) return emonk_init_failed(env);

    return emonk_ok(env, ret);
}

static ERL_NIF_TERM
emonk_eval(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    emonk_vm_t* vm = NULL;
    ErlNifBinary bin;
    
    if(argc != 2) return enif_make_badarg(env);
    
    if(!enif_get_resource(env, argv[0], EMONK_CTX, (void**) &vm))
    {
        return enif_make_badarg(env);
    }
    else
    {
        vm->env = env;
        vm->error = 0;
    }
    
    if(!enif_inspect_binary(env, argv[1], &bin))
    {
        return enif_make_badarg(env);
    }

    return vm_eval(env, vm, (char*) bin.data, bin.size);   
}

static ERL_NIF_TERM
emonk_call(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    emonk_vm_t* vm = NULL;
    ErlNifBinary bin;
    
    if(argc != 3) return enif_make_badarg(env);
    
    if(!enif_get_resource(env, argv[0], EMONK_CTX, (void**) &vm))
    {
        return enif_make_badarg(env);
    }
    else
    {
        vm->env = env;
        vm->error = 0;
    }

    return vm_call(env, vm, argv[1], argv[2]);
}

static ErlNifFunc emonk_funcs[] =
{
    {"new_context", 0, emonk_new_ctx},
    {"new_context", 1, emonk_new_ctx},
    {"eval", 2, emonk_eval},
    {"call", 3, emonk_call}
};

ERL_NIF_INIT(emonk, emonk_funcs, emonk_load, NULL, NULL, NULL)
