#!/usr/bin/env ruby

# Script to update CMakeLists.txt with TIM file entries
# 
# This script:
# 1. Finds all .tim files in the assets directory
# 2. Updates the CMakeLists.txt by adding psn00bsdk_target_incbin entries
#    between the #region images and #endregion markers

require 'fileutils'

# Path configuration
$script_dir = File.dirname(File.expand_path(__FILE__))
$tools_root = File.expand_path('..', $script_dir)
$project_root = File.expand_path('..', $tools_root)
$src_dir = File.join($project_root, 'src')
$assets_dir = File.join($src_dir, 'assets')
$assets_file = File.join($assets_dir, 'assets.h')
$cmake_file = File.join($src_dir, 'CMakeLists.txt')

def parcel_tim_files
   cmake_file = $cmake_file
   # Find all .tim files in the assets directory
   tim_files = Dir.glob(File.join($assets_dir, '*.tim')).map do |file|
   # Get the relative path from the CMakeLists.txt file location
   File.join('assets', File.basename(file))
   end

   puts "Found #{tim_files.length} TIM files:"
   tim_files.each { |file| puts "  - #{file}" }

   # Read the CMakeLists.txt file
   cmake_content = File.read(cmake_file)

   # Create the new content to insert
   cmake_includes = tim_files.map do |tim_file|
      target_name = File.basename(tim_file, '.tim')
      "psn00bsdk_target_incbin(hello_pong PRIVATE tim_#{target_name} #{tim_file})"
   end

   cmake_includes = cmake_includes.join("\n")

   # Find the region markers and replace the content between them
   if cmake_content.include?('#region images') && cmake_content.include?('#endregion')
      updated_content = cmake_content.gsub(
         /#region images\n.*?#endregion/m,
         "#region images\n#{cmake_includes}\n#endregion"
      )
      File.write(cmake_file, updated_content)
      puts "Successfully updated #{cmake_file} with #{tim_files.length} TIM file entries."
   else
      puts "Error: Could not find the region markers (#region images and #endregion) in #{cmake_file}"
      exit 1
   end

   # Update assets.h with the TIM file entries
   tim_entries = tim_files.map do |tim_file|
      target_name = File.basename(tim_file, '.tim')
      "extern u_long tim_#{target_name}[];\n"
   end

   puts "Include TIM file entries in main.c: \n"
   puts "// region images"
   puts tim_entries.join
   puts "// endregion "
end

parcel_tim_files if __FILE__ == $0