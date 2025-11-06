import matplotlib.pyplot as plt
import pandas as pd


df = pd.read_csv("my_score_vs_rmsd_t14.csv")

plt.scatter(df['SCORE'],df['RMSD'])
plt.xlabel("Total Score (REU)")
plt.ylabel(r"RMSD ($\AA$)")
plt.title("RMSD vs SCORE")
plt.savefig("lab7_plot_t14_2.png")
plt.show()