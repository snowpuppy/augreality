#!/usr/bin/env ruby

require 'date'
require 'fileutils'
require 'open-uri'
require 'hpricot'

# Configuration settings
absolute = 'https://engineering.purdue.edu/477grp5'
homedir = '/home/shay/a/477grp5/'
target = homedir + 'website/'
user, project = 'snowpuppy', 'augreality'
fetch = true

# Defines a week entry template
def dw(week_num)
	week_num = week_num.to_s.rjust(2, '0')
	"\t<!------ WEEK #{week_num} ------>
	<h3 id=\"week#{week_num}\">Week #{week_num}</h3>
"
end

# Ends a week entry template
def ew(week_num, accomplishments, wwt, pwt)
	week_num = week_num.to_s
	"\t<div class=\"summary\"><b>WEEK #{week_num} SUMMARY</b><br>
					   <u>Accomplishments</u>: <br>
					   #{accomplishments}<br>
					   <u>Weekly Work Total</u>: #{wwt} hours<br>
					   <u>Project Work Total</u>: #{pwt} hours</div>
"
end

# Defines a lab notebook entry template
def de(date, hours, content)
	date = date.strftime('%B %-d, %Y')
	# Pluralization handling
	s = (hours == 1) ? '' : 's'
	hours = hours.to_s
	"\t\t<div class=\"entry\"><b>#{date}, #{hours} hour#{s}</b>:<br>
		#{content}
		</div><p></p>
"
end

# Generates the start of week header if necessar
def git_startweek(a)
	cnt = ''
	# Week %N (%D) where Week 0 = Before this semester
	title = a.split(' ')
	if title.length > 1 and title.length < 4 then
		title[1].to_i
	else
		-1
	end
end

# Generates the end of week footer if necessary
def git_endweek(wn, acc, wwt, pwt)
	if wn >= 0 then
		puts "Detected week #{wn} with a total of #{wwt} hours"
		# Accomplishment store
		ew(wn, acc, wwt, pwt)
	else
		''
	end
end

# Generates a list of GIT entries
def git_doentries(a)
	cnt = ''
	# Go through each entry
	a.each do |m|
		date = nil
		# Get a date in either of 2 formats
		if not (/(?<date>[0-9]{1,2}\/[0-9]{1,2}\/[0-9]{2,4}) \((?<hours>[0-9]+) h[^)]+\)[:]? (?<content>.+)/mi =~ m.inner_html).nil? then
			date = Date::strptime(date, '%m/%d/%Y')
		elsif not (/(?<date>[A-Z]{3,4} [0-9]{1,2}, [0-9]{2,4}) \((?<hours>[0-9]+) h[^)]+\)[:]? (?<content>.+)/mi =~ m.inner_html).nil? then
			# Abbreviate the month name if not already so
			date = date.split(' ')
			date[0] = date[0][0..2]
			date = Date::strptime(date.join(' '), '%b %d, %Y')
		end
		cnt += de(date, hours.to_i, content) if not date.nil?
	end
	return cnt
end

# Fixes local links that wget misses so that relatives work in the nb directory
def fix_links(filename, absolute)
	search_for = filename.split('/')[-1]
 	text = File.read(filename) or fail "Could not open " + filename
	puts "Fixup #{filename} replacing #{search_for} with local"
	# Change relative links inside the same document
	text.gsub!(search_for + '#', '#')
	# Change absolute links to the thumbnail directory
	text.gsub!(absolute, '.')
	File.open(filename, "w") { |f| f.write(text) } or fail "Could not write" + filename
end

# Parses out a weekly or project work total
def weekly_work_total(text)
	wwt = 0
	/[Ww]ork [Tt]otal: (?<wwt>[0-9]+)/ =~ text
	if wwt.nil? or wwt.length < 1 then
		0
	else
		wwt.to_i
	end
end

# Updates the lab notebooks to the right format
def update_lab_notebook(nb)
	nb += '.html'
	puts 'Updating nb/' + nb
 	nbt = File.read('/home/shay/a/477grp5/' + nb) or fail 'Could not open template lab notebook'
	# Scanahead for snip site
	nbt = nbt.split('%%%%% SNIP %%%%%')
	fail 'Template file invalid' unless nbt.length == 2
	# Grab the source file
	cnt, pwt, srcfile, divdiv = '', 0, nil, nil
	/<!------ SOURCE: (?<srcfile>[^ ]+) ------>/ =~ nbt[0]
	fail 'Please mark the notebook source file name' if srcfile.nil? or srcfile.length < 2
	# Grab the heading type used for dividing
	/<!------ DIVIDER: (?<divdiv>[^ ]+) ------>/ =~ nbt[0]
	fail 'Select the heading type used for dividing sections' if divdiv.nil? or divdiv.length < 2
	# Now pull contents of the github nb
	File.open(srcfile) do |f|
		acc, wn, wwt = '', -1, 0
		# Parse the markdown body
		(Hpricot(f)/'div.markdown-body').first.each_child do |a|
			if a.name == divdiv then
				pwt += wwt
				# End week entry
				cnt += git_endweek(wn, acc, wwt, pwt)
				wn = git_startweek((a/'span/a').first.inner_html)
				# Start week entry
				cnt += dw(wn) if wn >= 0
				acc, wwt = '', 0
			elsif a.name == 'ul' then
				m = a/'/li'
				ee = git_doentries(m)
				if ee.nil? or ee.length < 1 then
					# Check for weekly work total
					m.each do |e|
						t = weekly_work_total(e.inner_text)
						if t > 0 then
							# Update total and kill bullet
							wwt = t if wwt == 0
							e.parent.children.delete(e)
						end
					end
					# No more lab entries, start accumulating
					acc += a.to_html
				else
					# Got valid lab entries, go ECE
					cnt += ee
				end
			elsif a.name == 'p' then
				# Possible weekly work total
				e = a.inner_text.strip
				t = weekly_work_total(e)
				m = a/'b'
				if t > 0 then
					# Got a hit
					wwt = t if wwt == 0
				elsif m.length != 1 or m.first.inner_text.strip != e
					# False positive
					acc += a.to_html
				end
			elsif a.name != 'b' then
				# Everything Else
				a = a.to_html
				acc += a if a.strip.length > 0
			end
		end
		# Clean up last entry
		pwt += wwt
		cnt += git_endweek(wn, acc, wwt, pwt)
	end
	# Fix images stored locally on the server thumbs directory
	cnt.gsub!(/(src|href)="\.\//, '\\1="../')
	File.open('nb/' + nb, 'w') do |f|
		# Gsub the links to fix the ../ problem on locals
		f.write(nbt[0] + cnt.gsub(/href="([A-Z])/, 'href="../\\1') + nbt[1])
	end
end

fail "No user/project provided" unless user and project
FileUtils.chdir(target)

if fetch then
	# Settings for pulling pages from the wiki
	wiki_folder = "#{user}/#{project}/wiki/"
	uri = 'https://github.com'
	xpath = 'div.markdown-body/ul/li/strong/a'
	command = 'wget -t 2 -k -c -r -l 1 -nd --input-file=url.txt'

	# Clean out the old pages
	Dir.foreach(target) do |filename|
		FileUtils.rm(filename) if filename[0] != '.' and not Dir.exist?(filename)
	end

	# Generate URL list of files to fetch
	File.open('url.txt', 'w') do |f|
		['_pages', '_history'].each { |p| f.puts("#{uri}/#{wiki_folder}#{p}") }
		(Hpricot(open("#{uri}/#{wiki_folder}_pages"))/xpath).each do |a|
			f.puts(uri + a[:href])
		end
	end
	# Actually pull the pages
	system(command)
	FileUtils.rm('url.txt')

	# Copy "index.htm" to "wiki" so that the references work out
	begin
		FileUtils.rm('index.htm')
	rescue
	end
	begin
		FileUtils.cp('wiki', 'index.htm')
	rescue
	end

	# Fix all local links
	Dir.foreach(target) do |filename|
		fix_links(filename, absolute) if filename[0] != '.' and not Dir.exist?(filename)
	end
end

# Lab Notebook Update
['carlson8', 'smith777', 'ellis15', 'green26'].each do |nb|
	update_lab_notebook(nb)
end
