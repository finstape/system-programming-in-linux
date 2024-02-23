from scapy.all import *
import requests
import customtkinter as ctk
import re


# Function to retrieve services data from a URL
def get_services():
    url = "https://raw.githubusercontent.com/nmap/nmap/master/nmap-services"
    try:
        response = requests.get(url)
        if response.status_code == 200:
            services_data = response.text
            return services_data
        else:
            print("Error while receiving these services:", response.status_code)
            return None
    except Exception as e:
        print("Error:", e)
        return None


# Function to parse services data and extract port-service mappings
def parse_services(data):
    services = {}
    if data:
        for line in data.splitlines():
            if not line.startswith("#"):
                parts = line.split()
                port = int(parts[1].split("/")[0])
                service_name = parts[0]
                services[port] = service_name
    return services


# Function to scan ports for a list of target IPs
def scan_ports(target_ips, ports):
    results = {}
    for ip in target_ips:
        for port in ports:
            response = sr1(IP(dst=ip) / TCP(dport=port, flags="S"), timeout=1, verbose=0)
            status = "Filtered"
            if response is not None:
                if response.haslayer(TCP):
                    if response.getlayer(TCP).flags == 0x12:
                        status = "Open"
                    elif response.getlayer(TCP).flags == 0x14:
                        status = "Closed"
            results[(ip, port)] = status
    return results


# Function to check if an IP is reachable (online)
def is_on_server(ip):
    if sr(IP(dst=ip) / ICMP(), timeout=1, verbose=0):
        return True
    return False


# Function to associate open ports with their corresponding services
def name_services(port_results, services):
    service_info = {}
    for (ip, port), status in port_results.items():
        if status == "Open":
            service_info.setdefault(ip, []).append(services.get(port, str(port)))
    return service_info


# Function to retrieve MAC address of an IP using ARP
def get_mac(ip_address):
    arp = ARP(pdst=ip_address)
    ether = Ether(dst="ff:ff:ff:ff:ff:ff")
    packet = ether / arp

    result = srp(packet, timeout=3, verbose=False)[0]
    mac_address = next((received.hwsrc for _, received in result), "N/A") if result else "N/A"
    return mac_address


# Function to retrieve host information including IP, ISP, country, region, city, and MAC address
def get_host(ip_address):
    try:
        response = requests.get(f'http://ip-api.com/json/{ip_address}')
        data = response.json()
        return {
            'IP Address': data['query'],
            'ISP': data['isp'],
            'Country': data['country'],
            'Region': data['regionName'],
            'City': data['city'],
            'MAC Address': get_mac(ip_address),
        }
    except Exception as e:
        return {
            'IP Address': ip_address,
            'ISP': 'N/A',
            'Country': 'N/A',
            'Region': 'N/A',
            'City': 'N/A',
            'MAC Address': 'N/A',
        }


# Function to perform IP scanning, retrieve services, and display results
def IP_scan(ips, ports, SERVICES, stats):
    services = name_services(scan_ports(ips, ports), SERVICES)
    host_infos = [get_host(ip) for ip in ips]

    result = stats + "Available servers: {}\n_________________________".format(len(host_infos))
    for host_info in host_infos:
        result += "\nIP Address: {}\nISP: {}\nCountry: {}\nRegion: {}\nCity: {}\nMAC Address: {}\nAvailable services: {}\n{}".format(
            host_info["IP Address"],
            host_info["ISP"],
            host_info["Country"],
            host_info["Region"],
            host_info["City"],
            host_info["MAC Address"],
            len(services.get(host_info["IP Address"], [])),
            ", ".join(services.get(host_info["IP Address"], []))
        )
        result += "\n_________________________"

    with open("results.txt", 'w') as f:
        f.write(result)

    return result


# Class for the graphical user interface
class Interface(ctk.CTk):
    def __init__(self):
        ctk.CTk.__init__(self)
        self.textbox = None
        self.start_button = None
        self.full_scan = None
        self.full_var = None
        self.ports = None
        self.ips = None
        self.ports_label = None
        self.ip_label = None
        self.services = parse_services(get_services())
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")
        self.title("IP Address Checker App")
        self.geometry("430x460")
        self.create_interface()

    def create_interface(self):
        self.ip_label = ctk.CTkLabel(self, text="IP Address(es):", text_color="white")
        self.ip_label.grid(row=0, column=0, pady=10)

        self.ports_label = ctk.CTkLabel(self, text="Port(s):", text_color="white")
        self.ports_label.grid(row=0, column=1, pady=10)

        self.ips = ctk.CTkTextbox(self, width=200, height=100)
        self.ips.grid(row=1, column=0, padx=10)

        self.ports = ctk.CTkTextbox(self, width=200, height=100)
        self.ports.grid(row=1, column=1)

        self.full_var = ctk.StringVar(value="off")
        self.full_scan = ctk.CTkCheckBox(self, text="Full scan", onvalue="on", offvalue="off", variable=self.full_var, command=self.set_event)
        self.full_scan.grid(row=2, columnspan=2, pady=10, padx=10)

        self.start_button = ctk.CTkButton(self, text="Check", command=self.scan)
        self.start_button.grid(row=3, columnspan=2, pady=10, padx=10)

        self.textbox = ctk.CTkTextbox(self, width=410, height=200)
        self.textbox.grid(row=4, columnspan=22, pady=10, padx=10)
        self.textbox.bind("<KeyPress>", self.prevent_typing)

    def prevent_typing(self, event):
        return "break"

    def set_event(self):
        if self.full_var.get() == "off":
            self.ports.configure(state="normal")
        else:
            self.ports.configure(state="disabled")

    # Function to initiate scanning process
    def scan(self):
        if self.full_var.get() == "on":
            PORTS = [int(x) for x in range(1, 65536)]
        else:
            port_values = self.ports.get("0.0", ctk.END)
            PORTS = []
            for port_input in port_values.split('\n'):
                port_input = port_input.strip()
                if not port_input:
                    continue
                if "[" in port_input and "]" in port_input:
                    match = re.match(r'\[(\d+)-(\d+)\]', port_input)
                    if match:
                        start = int(match.group(1))
                        end = int(match.group(2))
                        PORTS.extend(range(start, end + 1))
                else:
                    PORTS.append(int(port_input))

        IPs = []
        for ip in self.ips.get("0.0", ctk.END).split('\n'):
            ip_spl = [i.strip() for i in ip.split('.')]
            if len(ip_spl) == 4:
                if ip_spl[-1].startswith('*'):
                    start, stop = [int(i) for i in ip_spl[-1][2:-1].split('-')]
                    for i in range(start, stop + 1):
                        IPs.append(".".join(ip_spl[:3] + [str(i)]))
                else:
                    IPs.append(".".join(ip_spl))

        available_servers = list(filter(lambda IP: is_on_server(IP), IPs))
        port_results = scan_ports(available_servers, PORTS)
        host_count = len(available_servers)
        open_port_count = sum(status == "Open" for status in port_results.values())

        self.textbox.delete("1.0", ctk.END)

        stats = f"Statistics:\nTotal Hosts: {host_count}\nOpen Ports: {open_port_count}\n"
        self.textbox.insert(ctk.END, IP_scan(available_servers, PORTS, self.services, stats))


# Main block to run the GUI application
if __name__ == "__main__":
    # 147.45.72.190
    gui = Interface()
    gui.mainloop()
