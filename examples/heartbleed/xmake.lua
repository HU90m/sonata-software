-- Copyright lowRISC Contributors.
-- SPDX-License-Identifier: Apache-2.0

library("heartbleed_helpers")
   set_default(false)
   add_files("heartbleed.cc")

compartment("heartbleed")
    add_deps("debug", "heartbleed_helpers", "lcd")
    add_files("main.cc")

firmware("heartbleed_demo")
    add_deps("freestanding", "heartbleed")
    on_load(function(target)
        target:values_set("board", "$(board)")
        target:values_set("threads", {
            {
                compartment = "heartbleed",
                priority = 2,
                entry_point = "entry_point",
                stack_size = 0x1000,
                trusted_stack_frames = 3
            },
        }, {expand = false})
    end)
    after_link(convert_to_uf2)
