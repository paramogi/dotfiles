return {
	{
		"LazyVim/LazyVim",
		opts = {
			colorscheme = function()
				vim.cmd.colorscheme(vim.o.background == "light" and "tokyonight-day" or "tokyonight-moon")
			end,
		},
	},
}
