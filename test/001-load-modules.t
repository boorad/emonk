#!/usr/bin/env escript
% Licensed under the Apache License, Version 2.0 (the "License"); you may not
% use this file except in compliance with the License. You may obtain a copy of
% the License at
%
%   http://www.apache.org/licenses/LICENSE-2.0
%
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
% WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
% License for the specific language governing permissions and limitations under
% the License.

% Test that we can load each module.

main(_) ->
    etap:plan(3),
    Modules = [
        emonk,
        emonk_cache,
        emonk_sup
    ],

    LoadFun = fun(Module) ->
        etap_can:loaded_ok(Module, lists:concat(["Loaded: ", Module]))
    end,
    lists:foreach(LoadFun, Modules),
    etap:end_tests().
