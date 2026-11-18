# -*- coding: utf-8 -*-
"""Extrai da Tabela 1 da TACO (4a ed.) os 597 alimentos e gera alimentos.csv.

Formato: numero;descricao;energia_kcal;proteina_g;categoria_id  (UTF-8, separador ';')
As colunas sao identificadas pela posicao horizontal no PDF, de modo que celulas
vazias (ex.: alimentos 259, 450, 472, 516...) nao deslocam os valores.
Uso: py extrair_taco.py [caminho_do_pdf]
"""
import re
import sys

import pdfplumber

PDF = sys.argv[1] if len(sys.argv) > 1 else r"E:\Users\User\Downloads\taco_4_edicao_ampliada_e_revisada.pdf"
SAIDA = "alimentos.csv"

# (primeiro_id, ultimo_id, categoria_id) conforme as 15 categorias da TACO
FAIXAS = [
    (1, 63, 1), (64, 162, 2), (163, 258, 3), (259, 272, 4), (273, 322, 5),
    (323, 445, 6), (446, 469, 7), (470, 483, 8), (484, 490, 9), (491, 510, 10),
    (511, 519, 11), (520, 524, 12), (525, 556, 13), (557, 586, 14), (587, 597, 15),
]
X_KCAL = (410, 443)   # faixa horizontal (x0) da coluna Energia (kcal)
X_PROT = (475, 512)   # faixa horizontal (x0) da coluna Proteina (g)
X_DESC = (110, 375)   # faixa horizontal da descricao


def categoria_de(numero):
    for ini, fim, cat in FAIXAS:
        if ini <= numero <= fim:
            return cat
    raise ValueError("numero fora das faixas: %d" % numero)


def para_float(token):
    """Converte token da TACO para float; NA, Tr, * e vazio viram 0.0."""
    limpo = re.sub(r"[^0-9,.\-]", "", token.replace("*", ""))
    if limpo in ("", "-", ".", ","):
        return 0.0
    return float(limpo.replace(",", "."))


def main():
    registros = {}
    ultimo = None
    with pdfplumber.open(PDF) as pdf:
        for pagina in pdf.pages[27:72]:
            texto = pagina.extract_text() or ""
            if "Descrição dos alimentos" not in texto:
                continue  # paginas impares (minerais/vitaminas) nao interessam
            linhas = {}
            for w in pagina.extract_words():
                linhas.setdefault(round(w["top"] / 3), []).append(w)
            for chave in sorted(linhas):
                palavras = sorted(linhas[chave], key=lambda w: w["x0"])
                prim = palavras[0]
                desc = [w["text"] for w in palavras if X_DESC[0] <= w["x0"] and w["x1"] < X_DESC[1]]
                eh_item = (prim["x0"] < 105 and re.fullmatch(r"\d{1,3}", prim["text"])
                           and 1 <= int(prim["text"]) <= 597 and int(prim["text"]) not in registros)
                if eh_item:
                    numero = int(prim["text"])
                    kcal = [w["text"] for w in palavras if X_KCAL[0] <= w["x0"] < X_KCAL[1]]
                    prot = [w["text"] for w in palavras if X_PROT[0] <= w["x0"] < X_PROT[1]]
                    registros[numero] = [" ".join(desc),
                                         para_float(kcal[0]) if kcal else 0.0,
                                         para_float(prot[0]) if prot else 0.0,
                                         categoria_de(numero)]
                    ultimo = numero
                elif (ultimo is not None and desc and len(desc) == len(palavras)
                      and palavras[0]["x0"] >= X_DESC[0] and palavras[0]["text"][0].islower()
                      and chave != 0):
                    # continuacao de descricao quebrada em duas linhas
                    registros[ultimo][0] += " " + " ".join(desc)
    faltando = [n for n in range(1, 598) if n not in registros]
    if faltando:
        print("FALTANDO:", faltando)
        sys.exit(1)
    with open(SAIDA, "w", encoding="utf-8", newline="\n") as f:
        f.write("numero;descricao;energia_kcal;proteina_g;categoria_id\n")
        for n in range(1, 598):
            d, kcal, prot, cat = registros[n]
            f.write("%d;%s;%.1f;%.1f;%d\n" % (n, d.replace(";", ","), kcal, prot, cat))
    # validacao final
    with open(SAIDA, encoding="utf-8") as f:
        linhas_csv = f.read().splitlines()[1:]
    ids = [int(l.split(";")[0]) for l in linhas_csv]
    assert ids == list(range(1, 598)), "IDs invalidos"
    assert all(len(l.split(";")) == 5 for l in linhas_csv), "campos invalidos"
    print("OK: %d alimentos gravados em %s" % (len(linhas_csv), SAIDA))


if __name__ == "__main__":
    main()
