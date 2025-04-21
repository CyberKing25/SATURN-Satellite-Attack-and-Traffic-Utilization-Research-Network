import tkinter as tk
from tkinter import filedialog, messagebox
from scapy.all import rdpcap, IP, TCP, UDP
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import ipaddress


class PCAPAnalyzerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("PCAP Time Series Analyzer")
        self.root.geometry("1080x920")
        # Dark mode colors
        self.bg_color = "#2e2e2e"
        self.fg_color = "#ffffff"
        self.accent_color = "#444444"

        self.root.configure(bg=self.bg_color)
        self.energy_df = None
        self.packet_df = None

        self.selected_features = {
            'Timestamp': tk.BooleanVar(value=True),
            'Packet Size': tk.BooleanVar(value=True),
            'Source IP': tk.BooleanVar(value=True),
            'Destination IP': tk.BooleanVar(value=True),
            'Protocol': tk.BooleanVar(value=False),
            #'Energy Consumption': tk.BooleanVar(value=True),
        }
        self.title_label = tk.Label(
            root,
            text="Pcaps2Series",
            font=("Arial", 24, "bold"),
            fg="#39FF14",
            bg=self.bg_color
        )
        self.title_label.pack(pady=10)

        
        self.label = tk.Label(root, text="Choose a .pcap file to analyze", bg=self.bg_color, fg=self.fg_color, font=("Arial", 14))
        self.label.pack(pady=10)

        self.ip_range_label = tk.Label(root, text="Specify Normal IP Range (start - end):", bg=self.bg_color, fg=self.fg_color, font=("Arial", 12))
        self.ip_range_label.pack()

        self.start_ip_entry = tk.Entry(root, bg=self.accent_color, fg=self.fg_color, insertbackground=self.fg_color)
        self.start_ip_entry.insert(0, "10.0.0.1")
        self.start_ip_entry.pack(pady=2)

        self.end_ip_entry = tk.Entry(root, bg=self.accent_color, fg=self.fg_color, insertbackground=self.fg_color)
        self.end_ip_entry.insert(0, "10.0.0.5")
        self.end_ip_entry.pack(pady=2)

        self.feature_frame = tk.LabelFrame(root, text="Select features to export", bg=self.bg_color, fg=self.fg_color, padx=10, pady=10)
        self.feature_frame.pack(pady=10)

        for key, var in self.selected_features.items():
            cb = tk.Checkbutton(self.feature_frame,
                                    text=key,
                                    variable=var,
                                    bg=self.bg_color,
                                    fg=self.fg_color,
                                    selectcolor=self.accent_color,
                                    activebackground=self.bg_color,
                                    activeforeground=self.fg_color,
                                    font=("Arial", 12))
            cb.pack(anchor='w')

        self.select_button = tk.Button(root, text="Select PCAP File",  bg=self.accent_color, fg=self.fg_color, activebackground=self.fg_color, activeforeground=self.bg_color, command=self.load_pcap, font=("Arial", 12))
        self.select_button.pack(pady=10)

        self.label = tk.Label(root, text="Choose an Excel file with Energy Consumption", bg=self.bg_color, fg=self.fg_color, font=("Arial", 14))
        self.label.pack(pady=10)

        self.select_energy_button = tk.Button(root, text="Select Energy Consumption File",  bg=self.accent_color, fg=self.fg_color, activebackground=self.fg_color, activeforeground=self.bg_color, command=self.load_excel, font=("Arial", 12))
        self.select_energy_button.pack(pady=10)

        self.save_button = tk.Button(root, text="Save Extracted Data to CSV",  bg=self.accent_color, fg=self.fg_color, activebackground=self.fg_color, activeforeground=self.bg_color, command=self.save_csv, font=("Arial", 12), state=tk.DISABLED)
        self.save_button.pack(pady=10)


        self.canvas = None  # For matplotlib

    def load_pcap(self):
        file_path = filedialog.askopenfilename(title="Select a PCAP file", filetypes=[("PCAP files", "*.pcap *.pcapng")])
        if not file_path:
            return

        try:
            packets = rdpcap(file_path)
            data = []

            for pkt in packets:
                row = {}
                if self.selected_features['Timestamp'].get():
                    row['Timestamp'] = pd.to_datetime(float(pkt.time), unit='s')
                if self.selected_features['Packet Size'].get():
                    row['Packet Size'] = len(pkt)
                if self.selected_features['Source IP'].get() and IP in pkt:
                    row['Source IP'] = pkt[IP].src
                if self.selected_features['Destination IP'].get() and IP in pkt:
                    row['Destination IP'] = pkt[IP].dst
                if self.selected_features['Protocol'].get():
                    if TCP in pkt:
                        row['Protocol'] = 'TCP'
                    elif UDP in pkt:
                        row['Protocol'] = 'UDP'
                    else:
                        row['Protocol'] = 'Other'

                # Add traffic type based on IP range
                try:
                    start_ip = ipaddress.ip_address(self.start_ip_entry.get())
                    end_ip = ipaddress.ip_address(self.end_ip_entry.get())

                    ip_src = row.get('Source IP')
                    ip_dst = row.get('Destination IP')

                    def is_in_range(ip):
                        return start_ip <= ipaddress.ip_address(ip) <= end_ip

                    if not ip_src:
                        row['Traffic Type'] = 'Normal'  # Assume normal if no source IP
                    elif (is_in_range(ip_src)) and (ip_dst and is_in_range(ip_dst)):
                        row['Traffic Type'] = 'Normal'
                    else:
                        row['Traffic Type'] = 'Anomaly'
                except Exception:
                    row['Traffic Type'] = 'Unknown'

                data.append(row)


            self.packet_df = pd.DataFrame(data)

            if 'Timestamp' in self.packet_df.columns:
                # Convert to seconds (float) since start
                self.packet_df.sort_values(by='Timestamp', inplace=True)
                start_time = self.packet_df['Timestamp'].iloc[0]
                self.packet_df['Seconds'] = (self.packet_df['Timestamp'] - start_time).dt.total_seconds()

                # Drop datetime if you no longer need it
                self.packet_df.drop(columns='Timestamp', inplace=True)

                # Plot packets per second using floor'd Seconds
                resampled = self.packet_df.copy()
                resampled['SecInt'] = resampled['Seconds'].astype(int)
                packet_counts = resampled.groupby('SecInt').size()
                self.plot_data(packet_counts)


            self.save_button.config(state=tk.NORMAL)

        except Exception as e:
            messagebox.showerror("Error", f"Failed to process PCAP file:\n{e}")

    def load_excel(self):
        file_path = filedialog.askopenfilename(title="Select an Excel file", filetypes=[("Excel files", "*.xlsx *.xls")])
        if not file_path:
            return

        try:
            df = pd.read_excel(file_path)
            # Expect a time column - try to parse
            df['Timestamp'] = pd.to_datetime(df['Timestamp'], errors='coerce')
            df.dropna(subset=['Timestamp'], inplace=True)
            df.set_index('Timestamp', inplace=True)
            self.energy_df = df
            messagebox.showinfo("Success", "Energy data loaded successfully.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load Excel file:\n{e}")

    def save_csv(self):
        if self.packet_df is None:
            return

        df_to_save = self.packet_df.copy()

        # Merge energy data if available
        if self.energy_df is not None and 'Timestamp' in df_to_save.columns:
            df_to_save.set_index('Timestamp', inplace=True)
            merged_df = pd.merge_asof(df_to_save.sort_index(), self.energy_df.sort_index(), left_index=True, right_index=True, direction='nearest')
            df_to_save = merged_df.reset_index()
        else:
            df_to_save.reset_index(drop=False, inplace=True)

        # Drop any unselected features
        for col in list(df_to_save.columns):
            if col not in self.selected_features or not self.selected_features.get(col, tk.BooleanVar()).get():
                continue  # Keep timestamp/energy/etc. if they're in Excel
            if not self.selected_features[col].get():
                df_to_save.drop(columns=col, inplace=True, errors='ignore')

        file_path = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("CSV files", "*.csv")])
        if file_path:
            df_to_save.to_csv(file_path, index=False)
            messagebox.showinfo("Saved", f"Data saved to {file_path}")

    def plot_data(self, series):
        if self.canvas:
            self.canvas.get_tk_widget().destroy()

        fig, ax = plt.subplots(figsize=(6, 4))
        series.plot(ax=ax)
        ax.set_title("Packets per Second")
        ax.set_xlabel("Seconds since start (s)")
        ax.set_ylabel("Packet Count")

        self.canvas = FigureCanvasTkAgg(fig, master=self.root)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(pady=20)

# Run it
if __name__ == "__main__":
    root = tk.Tk()
    app = PCAPAnalyzerApp(root)
    root.mainloop()
