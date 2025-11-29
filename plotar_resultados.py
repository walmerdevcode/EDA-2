import pandas as pd
import matplotlib.pyplot as plt

# ===========================
#   CONFIGURAÇÕES GERAIS
# ===========================
plt.style.use("seaborn-v0_8-whitegrid")

cores = {
    "AVL": "#1f77b4",
    "RN": "#d62728",
    "B1": "#2ca02c",
    "B5": "#ff7f0e",
    "B10": "#9467bd",
}

espessura = 1.0


# ===========================
#    CARREGAR OS ARQUIVOS CSV
# ===========================
print("Carregando arquivos CSV...")

df_ins = pd.read_csv("resultados_insercao.csv")
df_rem = pd.read_csv("resultados_remocao.csv")

print("CSV carregados.")


# ===========================
#     FUNÇÃO QUE PLOTA OS GRÁFICOS
# ===========================
def gerar_grafico(df, titulo, nome_pdf):
    plt.figure(figsize=(11, 6))

    plt.plot(df["n"], df["AVL"], label="AVL",
             linewidth=espessura, color=cores["AVL"])

    plt.plot(df["n"], df["RN"], label="Rubro-Negra",
             linewidth=espessura, color=cores["RN"])

    plt.plot(df["n"], df["B1"], label="Árvore B (t=1)",
             linewidth=espessura, color=cores["B1"])

    plt.plot(df["n"], df["B5"], label="Árvore B (t=5)",
             linewidth=espessura, color=cores["B5"])

    plt.plot(df["n"], df["B10"], label="Árvore B (t=10)",
             linewidth=espessura, color=cores["B10"])

    plt.title(titulo, fontsize=16, fontweight="bold")
    plt.xlabel("Tamanho do Conjunto (n)", fontsize=14)
    plt.ylabel("Esforço da Operação", fontsize=14)

    plt.legend(fontsize=11)
    plt.grid(True, linewidth=0.3, alpha=0.6)

    plt.tight_layout()
    plt.savefig(nome_pdf)
    print(f"✔ PDF salvo: {nome_pdf}")

    plt.show()


# ===========================
#   GERAÇÃO DOS GRÁFICOS
# ===========================
gerar_grafico(
    df_ins,
    "Esforço de Inserção nas Árvores (AVL, Rubro-Negra, B-Tree)",
    "grafico_insercao.pdf"
)

gerar_grafico(
    df_rem,
    "Esforço de Remoção nas Árvores (AVL, Rubro-Negra, B-Tree)",
    "grafico_remocao.pdf"
)
