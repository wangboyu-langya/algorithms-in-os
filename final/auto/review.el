(TeX-add-style-hook
 "review"
 (lambda ()
   (setq TeX-command-extra-options
         "-shell-escape")
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "a4paper")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("inputenc" "utf8") ("caption" "justification=centering")))
   (add-to-list 'LaTeX-verbatim-environments-local "lstlisting")
   (add-to-list 'LaTeX-verbatim-environments-local "minted")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "lstinline")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "lstinline")
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art10"
    "inputenc"
    "fullpage"
    "listings"
    "xcolor"
    "minted"
    "sectsty"
    "pdfpages"
    "graphicx"
    "luatexja-fontspec"
    "fontspec"
    "amsmath"
    "amssymb"
    "amsthm"
    "upgreek"
    "siunitx"
    "caption"
    "subcaption"
    "indentfirst"
    "cite"
    "pdfsync")
   (TeX-add-symbols
    '("emphasis" 1)
    '("problem" 1)
    "andname")
   (LaTeX-add-labels
    "safe"
    "deadlocksummary"
    "thread"
    "cpuutil"
    "fig:disklayout"
    "longfilename"
    "staticlink"))
 :latex)

