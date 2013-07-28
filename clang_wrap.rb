#!/usr/bin/env ruby

CLANG='clang'
OPT='opt-3.2'
LLC='llc-3.2'

def my_system(*a)
  p a
  r = system(*a)
  if !r
    exit(1)
  end
end

use_plugin = false

argv = ARGV.dup
i = nil
# Clang crashes for Ruby's bignum.c. I don't know why.
if (i = argv.index('-c')) && (j = argv.index('-o')) && !argv.index('bignum.c')
  argv[i] = '-S'
  ot = argv[j+1]
  argv[j+1] += '.s'
  argv.push('-emit-llvm')
  use_plugin = true
end

my_system(CLANG, *argv)

if use_plugin
  bc = argv[j+1] + '.bc'
  my_system(OPT, '-load=' + File.dirname(__FILE__) + '/clearstack.so',
            '-clearstack', argv[j+1], '-o', bc)
  my_system(LLC, bc, '-o', bc + '.s', '-relocation-model=pic')
  my_system(CLANG, bc + '.s', '-c', '-o', ot)
end
