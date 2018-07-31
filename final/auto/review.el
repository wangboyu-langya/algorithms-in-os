(TeX-add-style-hook
 "review"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "a4paper")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("inputenc" "utf8") ("caption" "justification=centering")))
   (add-to-list 'LaTeX-verbatim-environments-local "minted")
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art10"
    "inputenc"
    "fullpage"
    "listings"
    "xcolor"
    "sectsty"
    "pdfpages"
    "luatexja-fontspec"
    "fontspec"
    "amsmath"
    "amssymb"
    "amsthm"
    "upgreek"
    "siunitx"
    "graphicx"
    "enumitem"
    "caption"
    "subcaption"
    "indentfirst"
    "cite"
    "pdfsync")
   (TeX-add-symbols
    "andname"))
 :latex)

