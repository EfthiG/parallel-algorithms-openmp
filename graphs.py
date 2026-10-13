import os
import numpy as np
import matplotlib.pyplot as plt

def read_gauss_data(lines):
    data = dict()

    for line in lines[1:]:
        parts = line.split()
        if len(parts) != 6:
            continue

        size = int(parts[0])
        method = parts[1]
        order = parts[2]
        threads = int(parts[3])
        time = float(parts[4])
        schedule = parts[5]

        key = (size, threads, order, schedule)
        if key not in data:
            data[key] = []
        data[key].append(time)

    # Calculate the average time for each key
    for key in data:
        data[key] = np.mean(data[key])

    return data


def read_game_of_life_data(lines):
    data = {}

    for line in lines[1:]:
        parts = line.split()
        if len(parts) != 6:
            continue

        rows = int(parts[0])
        cols = int(parts[1])
        generations = int(parts[2])
        parallel = int(parts[3])
        threads = int(parts[4])
        time = float(parts[5])

        key = (rows, cols, generations, parallel, threads)
        if key not in data:
            data[key] = []
        data[key].append(time)

    # Calculate the average time for each key
    for key in data:
        data[key] = np.mean(data[key])

    return data


def read_data(text_file):
    data = []
    with open(text_file, 'r') as file:
        lines = file.readlines()

    if "gauss" in text_file:
        return read_gauss_data(lines)
    elif "game_of_life" in text_file:
        return read_game_of_life_data(lines)
    else:
        return None


def plot_gauss_data(data):
    sizes = [100, 1000, 5000, 10000]
    threads = [1, 2, 4, 8, 16]
    orders = ['by_line', 'by_column']
    labels_order = ['1', '2', '4', '8', '16']
    schedules = ['default', 'static', 'dynamic', 'guided']
    schedule_labels = ['Default', 'Static', 'Dynamic', 'Guided']
    by_line_color = '#003f5c'
    by_column_color = '#2f9e44'

    # Plot by algorithm
    for size in sizes:
        times_by_line = []
        times_by_column = []
        for t in threads:
            if t == 1:
                key_by_line = (size, t, 'by_line', 'default')
                key_by_column = (size, t, 'by_column', 'default')
            else:
                key_by_line = (size, t, 'by_line', 'static')
                key_by_column = (size, t, 'by_column', 'static')
            
            times_by_line.append(data.get(key_by_line, 0))
            times_by_column.append(data.get(key_by_column, 0))

        x = np.arange(len(labels_order))  # the label locations
        width = 0.35  # the width of the bars

        fig, ax = plt.subplots()
        rects1 = ax.bar(x - width/2, times_by_line, width, label='By Line', color=by_line_color)
        rects2 = ax.bar(x + width/2, times_by_column, width, label='By Column', color=by_column_color)

        # Add some text for labels, title and custom x-axis tick labels, etc.
        ax.set_xlabel('Threads')
        ax.set_ylabel('Time (s)')
        ax.set_title(f'Execution Time for Size {size}')
        ax.set_xticks(x)
        ax.set_xticklabels(labels_order)
        ax.legend()

        fig.tight_layout()

        plt.savefig(f"results/gauss_{size}_by_algorithm.png")


    # Plot by schedule
    bar_width = 0.15  # Width for each bar
    group_gap = 0.3  # Reduced gap between thread groups

    # Define color shades
    by_line_colors = ['#003f5c', '#2f4b7c', '#665191']  # Dark to light blue
    by_column_colors = ['#2f9e44', '#43aa8b', '#90be6d']  # Dark to light green

    for size in sizes:
        fig, ax = plt.subplots(figsize=(12, 7))

        # Calculate x positions for each thread group
        x_positions = np.arange(len(threads)) * (2 * bar_width * len(schedules) + group_gap)

        for thread_index, thread in enumerate(threads):
            base_position = x_positions[thread_index]

            if thread == 1:  # Handle single-threaded case
                by_line_time = data.get((size, thread, 'by_line', 'default'), 0)
                by_column_time = data.get((size, thread, 'by_column', 'default'), 0)

                ax.bar(
                    base_position - bar_width / 2,
                    by_line_time,
                    bar_width,
                    color=by_line_colors[1],
                    label='By Line - Default' if thread_index == 0 else "",
                )
                ax.bar(
                    base_position + bar_width / 2,
                    by_column_time,
                    bar_width,
                    color=by_column_colors[1],
                    label='By Column - Default' if thread_index == 0 else "",
                )
            else:
                for schedule_index, schedule in enumerate(['static', 'dynamic', 'guided']):
                    offset = -1.5 * bar_width + schedule_index * bar_width

                    # Plot by_line bars
                    by_line_time = data.get((size, thread, 'by_line', schedule), 0)
                    ax.bar(
                        base_position + offset,
                        by_line_time,
                        bar_width,
                        color=by_line_colors[schedule_index],
                        label=f'By Line - {schedule.capitalize()}' if thread_index == 0 else "",
                    )

                    # Plot by_column bars
                    by_column_time = data.get((size, thread, 'by_column', schedule), 0)
                    ax.bar(
                        base_position + offset + 3.5 * bar_width,
                        by_column_time,
                        bar_width,
                        color=by_column_colors[schedule_index],
                        label=f'By Column - {schedule.capitalize()}' if thread_index == 0 else "",
                    )

        # Customize the plot
        ax.set_xticks(x_positions)
        ax.set_xticklabels([str(t) for t in threads])
        ax.set_xlabel('Threads')
        ax.set_ylabel('Execution Time (s)')
        ax.set_title(f'Execution Time Comparison (Size: {size})')
        ax.grid(axis='y', linestyle='--', alpha=0.7)

        # Create custom legend entries for schedules
        schedule_legend = [
            plt.Line2D([0], [0], color=by_line_colors[0], lw=8, label='Static (By Line)'),
            plt.Line2D([0], [0], color=by_line_colors[1], lw=8, label='Dynamic (By Line)'),
            plt.Line2D([0], [0], color=by_line_colors[2], lw=8, label='Guided (By Line)'),
            plt.Line2D([0], [0], color=by_column_colors[0], lw=8, label='Static (By Column)'),
            plt.Line2D([0], [0], color=by_column_colors[1], lw=8, label='Dynamic (By Column)'),
            plt.Line2D([0], [0], color=by_column_colors[2], lw=8, label='Guided (By Column)'),
        ]

        # Place the legend on the right
        ax.legend(
            handles=schedule_legend,
            loc='center left',
            bbox_to_anchor=(1, 0.5),
            title='Schedules and Algorithms'
        )

        # Save the plot
        plt.tight_layout()
        plt.savefig(f'results/gauss_{size}_by_schedule.png')
        plt.close()


def plot_game_of_life_data(data):
    sizes = [64, 1024, 2048]
    threads = [1, 2, 4, 8, 16]
    labels = ['1', '2', '4', '8', '16']

    for size in sizes:
        times = []
        for t in threads:
            if t == 1:
                key = (size, size, 1000, 0, 1)
                times.append(data.get(key, 0))
            else:
                key = (size, size, 1000, 1, t)
                times.append(data.get(key, 0))

        plt.figure()
        plt.bar(labels, times, color=['#003f5c', '#43aa8b', '#2f4b7c', '#2f9e44', '#665191'])
        # plt.plot(labels, times, marker='o')
        plt.xlabel('Threads')
        plt.ylabel('Time (s)')
        plt.title(f'Execution Time for Size {size}x{size}')
        plt.savefig(f"results/game_of_life_{size}.png")


def plot_data(data, text_file):
    if "gauss" in text_file:
        plot_gauss_data(data)
    elif "game_of_life" in text_file:
        plot_game_of_life_data(data)
    else:
        return None


if __name__ == "__main__":
    results_folder = "results/"
    results_files = [
        'gauss.txt',
        'game_of_life.txt'
    ]

    for text_file in results_files:
        text_file = results_folder + text_file

        if not os.path.exists(text_file):
            print(f"File {text_file} does not exist")
            continue

        data = read_data(text_file)
        plot_data(data, text_file)