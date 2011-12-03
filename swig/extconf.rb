#!/usr/bin/env ruby
require 'mkmf'

def crash(str)
  print("  extconf failure: %s\n" % str)
  exit 1
end

dir_config('cueify')
unless have_library('cueify', 'cueify_device_new')
  crash('need cueify')
end

create_makefile('cueify')
