import pandas as pd
import matplotlib.pyplot as plt
import os
import random
from datetime import datetime, timedelta

CSV_FILE = 'tracker_data.csv'

def generate_dummy_data():
    """Generates 7 days of historical data for the charts if no data exists."""
    dates = [(datetime.now() - timedelta(days=i)).strftime('%Y-%m-%d') for i in range(6, -1, -1)]
    data = {
        'Date': dates,
        'XP_Earned': [random.randint(20, 150) for _ in range(7)],
        'Tasks_Completed': [random.randint(1, 5) for _ in range(7)],
        'Tasks_Missed': [random.randint(0, 2) for _ in range(7)]
    }
    df = pd.DataFrame(data)
    df.to_csv(CSV_FILE, index=False)
    return df

def main():
    """Load tracker data and display the HabitQuest analytics dashboard."""
    print("Initializing Visual Analytics Engine...")
    
    # FIX: Check if the file exists AND if it has actual data in it (> 0 bytes)
    if not os.path.exists(CSV_FILE) or os.path.getsize(CSV_FILE) == 0:
        df = generate_dummy_data()
    else:
        try:
            df = pd.read_csv(CSV_FILE)
        except (
            pd.errors.EmptyDataError,
            pd.errors.ParserError,
            OSError,
            UnicodeDecodeError
        ):
            df = generate_dummy_data()

    required_columns = [
        'Date',
        'XP_Earned',
        'Tasks_Completed',
        'Tasks_Missed'
    ]

    missing_columns = [
        col for col in required_columns
            if col not in df.columns
    ]

    if missing_columns:
        print(f"Invalid tracker data. Missing columns: {missing_columns}")
        return

    # Set up the Matplotlib Figure with 2 subplots
    plt.style.use('dark_background')
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    fig.canvas.manager.set_window_title('HabitQuest Analytics Dashboard')

    # Chart 1: 7-Day XP Trend (Line Chart)
    ax1.plot(df['Date'], df['XP_Earned'], marker='o', color='#00ffcc', linewidth=2, markersize=8)
    ax1.set_title('7-Day XP Progression', fontsize=14, color='white')
    ax1.set_xlabel('Date')
    ax1.set_ylabel('XP Earned')
    ax1.tick_params(axis='x', labelrotation=45)
    for label in ax1.get_xticklabels():
    label.set_ha('right')
    ax1.grid(color='#333333', linestyle='--', alpha=0.7)

    # Chart 2: Task Completion Ratio (Pie Chart)
    total_completed = df['Tasks_Completed'].sum()
    total_missed = df['Tasks_Missed'].sum()
    
    labels = ['Completed', 'Missed']
    sizes = [total_completed, total_missed]
    colors = ['#00ffcc', '#ff3366']
    explode = (0.1, 0)

    ax2.pie(sizes, explode=explode, labels=labels, colors=colors, autopct='%1.1f%%', 
            shadow=True, startangle=90, textprops={'color': 'white', 'fontsize': 12})
    ax2.set_title('Overall Task Success Rate', fontsize=14, color='white')

    # Adjust layout and display
    fig.tight_layout(pad=2.0)
    plt.show()

if __name__ == "__main__":
    main()
