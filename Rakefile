require 'rubygems/package_task'
require 'rake/extensiontask'

# Make sure cueifyRUBY_wrap.cxx exists.
if not File.exists?('build/swig/ruby/cueifyRUBY_wrap.cxx')
  if not File.exists('build')
    Dir.mkdir('build')
  end
  Dir.chdir('build')
  system('cmake ..')
  system('make ruby-cueify')
  Dir.chdir('..')
end

# Make sure that the file was generated with an appropriate version of SWIG.
if File.exists?('build/swig/ruby/cueifyRUBY_wrap.cxx')
  f = File.open('build/swig/ruby/cueifyRUBY_wrap.cxx', 'r')
  if f.readlines[2] =~ / \* Version ((?:[0-9]+\.?)+)/
    if $1 < "2.0.1" and RUBY_VERSION >= "1.9.0"
      print "ERROR: SWIG >= 2.0.1 required for Ruby >= 1.9.0\n"
      print "       Install a newer version of SWIG and then rerun 'make ruby-cueify'\n"
      exit 1
    end
  else
    print "Can't figure out what version of SWIG was used. Proceed with caution!\n"
  end
else
  print "Could not prepare cueify bindings.\n"
  exit 1
end

spec = Gem::Specification.new do |s|
  s.name = 'cueify'
  s.version = '0.5.0'
  s.summary = 'Ruby bindings for libcueify, a CD-metadata library'
  s.description = <<-EOF
  cueify provides access to a vast variety of Audio CD metadata,
  including Table of Contents (TOC) data, multi-session data, and
  CD-TEXT data.
EOF
  s.author = 'Ian Jacobi'
  s.email = 'pipian@pipian.com'
  s.platform = Gem::Platform::RUBY
  s.license = 'MIT'
  s.homepage = 'https://github.com/pipian/libcueify'
  s.extensions = FileList['build/swig/ruby/extconf.rb']
end

Gem::PackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new do |ext|
  ext.name = 'cueify'
  ext.ext_dir = 'build/swig/ruby'
  ext.tmp_dir = 'build/swig/ruby'
  ext.gem_spec = spec
  # rake-compiler doesn't support rake arguments directly in 0.7.9 and below
  ext.config_options = ARGV.select { |i| i =~ /\A--?/ }
end
