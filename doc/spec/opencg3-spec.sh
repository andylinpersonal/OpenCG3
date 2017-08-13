#!/bin/bash

reset
name="opencg3-spec"

cd "$(dirname "$0")"
rm "$name.log" 2>'/dev/null'

export max_print_line=4096
filtered="v1.42, 2010/05/14 <tvz>`
		`\|For additional information on amsmath, use the \`?' option.`
		`\|ABD: EveryShipout initializing macros"

function texcomp
{	pdflatex $1 -halt-on-error "$name.tex" | \
		sed "s/\([0-9A-Za-z.\/\-]*[.]\(cfg\|clo\|cls\|def\|dict\|fd\|mkii\|sty\|tex\)\)\|(\|)//g" | sed "s/\[[^\]*\]//g" | \
		sed 's/^[ \t]*//' | sed 's/[ \t]*$//' | tr -s ' ' | grep -v "$filtered" | grep '\S'
}  

texcomp -draftmode && echo '' && \
[[ -z "$(cat "$name.log" | grep 'Fatal error')" ]] && \
texcomp | grep --color=none 'Output written\|Transcript written' && echo ''

rm -rf *.aux *.log *.nav *.out *.snm *.toc *.vrb *~ 2>&1 >'/dev/null'
