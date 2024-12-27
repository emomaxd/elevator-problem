import json
import matplotlib.pyplot as plt

def visualize_results(file_path, output_image="results_plot.png"):
    with open(file_path, "r") as file:
        data = json.load(file)

    algorithms = data.keys()
    test_sizes = [int(size) for size in next(iter(data.values())).keys()]

    averages = {algorithm: [
        sum(values) / len(values) if isinstance(values, list) else values
        for values in (data[algorithm][str(size)] for size in test_sizes)
    ] for algorithm in algorithms}

    plt.figure(figsize=(12, 8))

    for algorithm in algorithms:
        plt.plot(test_sizes, averages[algorithm], marker="o", label=algorithm)

    plt.title("Elevator Scheduling Algorithms Performance", fontsize=16)
    plt.xlabel("Number of Requests", fontsize=14)
    plt.ylabel("Average Time (seconds)", fontsize=14)
    plt.xscale("log")
    plt.yscale("log")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend(fontsize=12)
    plt.tight_layout()
    plt.savefig(output_image)
    plt.show()

if __name__ == "__main__":
    visualize_results("results.json")
