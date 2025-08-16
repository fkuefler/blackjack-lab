import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import sys
import numpy as np


# Function to generate a strategy chart given a CSV file
def generate_chart(csv_filename):
    try:
        # Extract rules from the CSV
        rules_text_list = []
        with open(csv_filename, "r") as file:
            for line in file:
                if line.startswith("#"):
                    rules_text_list.append(line.lstrip("#;").strip())
                else:
                    break
        rules_text = "\n".join(rules_text_list)

        # Read the CSV, skipping the commented metadata lines
        df = pd.read_csv(csv_filename, skiprows=7, comment="#")

        # Define a fixed, canonical order for all possible actions
        canonical_actions = ["Hit", "Stand", "Double", "Split", "Surrender"]
        unique_actions_in_data = set(df["Optimal Action"].unique())

        action_mapping = {}
        colors = []
        action_abbreviations = {}

        # Define full action abbreviations
        full_action_abbreviations = {
            "Hit": "H",
            "Stand": "S",
            "Double": "D",
            "Split": "Sp",
            "Surrender": "R",
        }
        # Define a full color map for all actions
        full_color_map = {
            "Hit": "#4CAF50",
            "Stand": "#F44336",
            "Double": "#FFEB3B",
            "Split": "#2196F3",
            "Surrender": "#607D8B",
        }

        # Build the action mapping and colors list based on the canonical order
        # This ensures that actions always map to the same number, and the colors list is
        # always in sync with the data.
        current_map_index = 0
        for action in canonical_actions:
            if action in unique_actions_in_data:
                action_mapping[action] = current_map_index
                action_abbreviations[action] = full_action_abbreviations[action]
                colors.append(full_color_map[action])
                current_map_index += 1

        # Apply the numerical mapping to the DataFrame
        df["Optimal_Action_Num"] = df["Optimal Action"].map(action_mapping)

        # Create pivot tables for the actions and their text representations
        action_pivot = df.pivot(
            index="Player Hand",
            columns="Dealer Upcard",
            values="Optimal_Action_Num",
        )
        action_text_pivot = df.pivot(
            index="Player Hand",
            columns="Dealer Upcard",
            values="Optimal Action",
        )

        # Define player hand order
        player_hand_order = [
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19",
            "A,2",
            "A,3",
            "A,4",
            "A,5",
            "A,6",
            "A,7",
            "A,8",
            "A,9",
            "2,2",
            "3,3",
            "4,4",
            "5,5",
            "6,6",
            "7,7",
            "8,8",
            "9,9",
            "10,10",
            "A,A",
        ]

        # Define dealer upcard order
        dealer_upcard_order = ["2", "3", "4", "5", "6", "7", "8", "9", "10", "A"]

        # Reindex the pivot tables to ensure consistent ordering
        action_pivot = action_pivot.reindex(
            index=player_hand_order, columns=dealer_upcard_order
        )
        action_text_pivot = action_text_pivot.reindex(
            index=player_hand_order, columns=dealer_upcard_order
        )

        # Create a custom colormap
        cmap = mcolors.ListedColormap(colors)

        # Create the heatmap
        plt.figure(figsize=(7, 8))
        ax = plt.gca()

        im = ax.imshow(action_pivot, cmap=cmap, interpolation="nearest", aspect="auto")

        plt.xlabel("Dealer Upcard", fontsize=12)
        plt.ylabel("Player Hand", fontsize=12)
        ax.set_xticks(np.arange(action_pivot.shape[1] + 1) - 0.5, minor=True)
        ax.set_yticks(np.arange(action_pivot.shape[0] + 1) - 0.5, minor=True)
        ax.grid(which="minor", color="black", linestyle="-", linewidth=1.5)
        ax.set_xticks(np.arange(action_pivot.shape[1]))
        ax.set_yticks(np.arange(action_pivot.shape[0]))
        ax.set_xticklabels(action_pivot.columns)
        ax.set_yticklabels(action_pivot.index)
        ax.xaxis.tick_top()
        ax.xaxis.set_label_position("top")

        # Annotate each cell with the action abbreviation
        for i in range(len(action_pivot.index)):
            for j in range(len(action_pivot.columns)):
                action_str = action_text_pivot.iloc[i, j]
                action_abbr = action_abbreviations.get(action_str, "")
                ax.text(
                    j,
                    i,
                    action_abbr,
                    ha="center",
                    va="center",
                    color="black",
                    fontsize=10,
                )

        # Create legend elements
        legend_elements = [
            plt.Rectangle((0, 0), 1, 1, fc=c, label=l)
            for l, c in zip(action_mapping.keys(), colors)
        ]
        # Create legend
        ax.legend(
            handles=legend_elements,
            title="Optimal Action",
            bbox_to_anchor=(1.05, 1),
            loc="upper left",
            edgecolor="black",
        )

        # Add rules text
        ax.text(
            1.05,
            0.75,
            rules_text,
            transform=ax.transAxes,
            fontsize=10,
            va="top",
            ha="left",
            bbox=dict(
                facecolor="white",
                alpha=0.8,
                edgecolor="grey",
                boxstyle="round,pad=0.5",
            ),
            linespacing=1.6,
        )

        # Add GitHub link
        ax.text(
            1.02,
            0.01,
            "github.com/fkuefler/blackjack-lab",
            ha="left",
            va="bottom",
            transform=ax.transAxes,
            fontsize=9,
            color="grey",
        )

        plt.tight_layout()
        # Save the figure if --save flag is present
        save_path = None
        if "--save" in sys.argv:
            save_index = sys.argv.index("--save")
            # Check if a filename is provided after --save
            if save_index + 1 < len(sys.argv) and not sys.argv[
                save_index + 1
            ].startswith("--"):
                save_path = sys.argv[save_index + 1]
            else:
                save_path = "blackjack_strategy_chart.png"

        if save_path:
            plt.savefig(save_path, bbox_inches="tight", dpi=600)
            print(f"Chart saved to {save_path}")
        plt.show()

    except FileNotFoundError:
        print(f"Error: The file '{csv_filename}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        csv_file = sys.argv[1]
        generate_chart(csv_file)
    else:
        print(
            "Usage: python chart_generator.py <path_to_csv_file> [--save [filename.png]]"
        )
