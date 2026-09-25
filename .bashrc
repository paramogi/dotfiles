# /etc/skel/.bashrc
#
# This file is sourced by all *interactive* bash shells on startup,
# including some apparently interactive shells such as scp and rcp
# that can't tolerate any output.  So make sure this doesn't display
# anything or bad things will happen !


# Test for an interactive shell.  There is no need to set anything
# past this point for scp and rcp, and it's important to refrain from
# outputting anything in those cases.
if [[ $- != *i* ]] ; then
	# Shell is non-interactive.  Be done now!
	return
fi

PS1="\[$(tput setaf 220)\]\w \[$(tput sgr0)\]\$ "
PS1='$(ip link show wg0 >/dev/null 2>&1 && printf "\001\033[38;5;46m\002[vpn]\001\033[0m\002 ")'"$PS1"

# Put your fun stuff here.

alias so='source'

alias v='nvim'
alias vi='nvim'
alias vim='nvim'

alias gcl='git clone'
alias gs='git status'
alias ga='git add'
alias gaa='git add --all'
alias gc='git commit'
alias gcm='git commit -m'
alias grs='git restore --staged'
alias gr='git restore'
alias gp='git push'
alias gpl='git pull'
alias gd='git diff'
alias gds='git diff --staged'
alias gl='git log'

alias py='python3'

export PATH=$PATH:/home/mogi/.local/bin
