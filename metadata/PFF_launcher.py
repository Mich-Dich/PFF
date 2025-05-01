import tkinter as tk
import ttkbootstrap as ttk
from ttkbootstrap import Style
from PIL import Image, ImageTk
from dataclasses import dataclass
import os
import yaml


@dataclass
class version:
    magor: int
    minor: int
    patch: int

@dataclass
class timestamp:
    year: int                                                                               # Full year (e.g., 2025)
    month: int                                                                              # Month (1-12)
    day: int                                                                                # Day of month (1-31)
    day_of_week: int                                                                        # Day of week (0-6, where 0=Sunday)
    hour: int                                                                               # Hour (0-23)
    minute: int                                                                             # Minute (0-59)
    secund: int                                                                             # Second (0-59)
    millisecend: int                                                                        # Millisecond (0-999)
    
@dataclass
class project_data:
    title: str                                                                              # called "display_name" in .pffproj
    description: str
    engine_version: version
    last_modified: timestamp



current_screen = 0
CONFIG_DIR = os.path.join(os.path.dirname(__file__), "config")
CONFIG_FILE = os.path.join(CONFIG_DIR, "saved.yaml")
TILE_WIDTH = 385                                                                            # Approximate width of each tile including padding

def set_screen(screen_index):
    global current_screen
    current_screen = screen_index
    if current_screen == 0:
        BU_projects.configure(style='primary.TButton')
        BU_library.configure(style='secondary.TButton')
    elif current_screen == 1:
        BU_projects.configure(style='secondary.TButton')
        BU_library.configure(style='primary.TButton')


def create_project_tile(parent, title, description, timestamp_text, image):
    frame       = ttk.Frame(parent, padding=10, style="secondary.TFrame")
    img_label   = ttk.Label(frame, image=image)
    img_label.image = image                                                                 # Keep reference
    img_label.grid(row=0, column=0, rowspan=3, sticky="n")
    base_wrap = 120
    title_label = ttk.Label(frame, text=title, style="h5.TLabel", wraplength=base_wrap)
    desc_label  = ttk.Label(frame, text=description, style="TLabel", wraplength=base_wrap)
    time_label  = ttk.Label(frame, text=timestamp_text, style="secondary.TLabel")
    title_label.grid(row=0, column=1, sticky="w", padx=10)
    desc_label.grid(row=1, column=1, sticky="w", padx=10)
    time_label.grid(row=2, column=1, sticky="w", padx=10)
    frame.title_label = title_label                                                         # Attach labels and base wraps to frame for later resizing
    frame.desc_label = desc_label
    frame.base_title_wrap = base_wrap
    frame.base_desc_wrap  = base_wrap
    return frame


def update_grid():
    for widget in project_grid_frame.winfo_children():
        widget.grid_forget()
    content_width = canvas_projects.winfo_width()
    if content_width == 1:
        return
    columns = max(1, content_width // TILE_WIDTH)
    text_additional_width = (1 + ((content_width / TILE_WIDTH) % 1) * TILE_WIDTH) / columns
    for index, tile in enumerate(project_tiles):
        if hasattr(tile, 'title_label'):
            new_wrap = tile.base_title_wrap + text_additional_width
            tile.title_label.configure(wraplength=new_wrap)
        if hasattr(tile, 'desc_label'):
            new_wrap = tile.base_desc_wrap + text_additional_width
            tile.desc_label.configure(wraplength=new_wrap)
        row = index // columns
        column = index % columns
        tile.grid(row=row, column=column, padx=10, pady=10, sticky="nsew")

def on_resize(event):
    canvas.coords(sidebar, 0, 0, sidebar_width, event.height)
    canvas.itemconfig(project_button_window, width=sidebar_width)
    canvas.itemconfig(lib_button_window, width=sidebar_width)
    canvas.coords(logo_item, sidebar_width // 2, sidebar_width // 2)
    update_grid()
    

if __name__ == "__main__":

    os.makedirs(CONFIG_DIR, exist_ok=True)
    if not os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, "w") as f:
            yaml.dump({"directories": []}, f)

    root = tk.Tk()
    root.title("PFF Launcher")
    root.geometry("1024x720")
    style = Style(theme='darkly')
    canvas = ttk.Canvas(root, highlightthickness=0)
    canvas.pack(fill="both", expand=True)

    # --- sidebar ---
    sidebar_width = 200
    sidebar = canvas.create_rectangle(0,0, sidebar_width, 720, fill="#181818", outline="")
    logo_img                = ImageTk.PhotoImage(Image.open("assets/Logo.png").resize((sidebar_width, sidebar_width), Image.Resampling.LANCZOS))
    logo_item               = canvas.create_image(sidebar_width // 2, sidebar_width // 2, image=logo_img)
    project_img             = ImageTk.PhotoImage(Image.open("assets/projects.png").resize((16, 16), Image.Resampling.LANCZOS))
    lib_img                 = ImageTk.PhotoImage(Image.open("assets/library.png").resize((16, 16), Image.Resampling.LANCZOS))
    BU_projects             = ttk.Button(root, text='Projects', image=project_img, compound='left', style='primary.TButton', takefocus=False, command=lambda: set_screen(0))
    BU_library              = ttk.Button(root, text='Library', image=lib_img, compound='left', style='secondary.TButton', takefocus=False, command=lambda: set_screen(1))
    BU_projects.image       = project_img
    BU_library.image        = lib_img
    project_button_window   = canvas.create_window(0, 230, window=BU_projects, anchor="nw", width=sidebar_width)
    lib_button_window       = canvas.create_window(0, 270, window=BU_library, anchor="nw", width=sidebar_width)

    # --- content area ---
    content_frame = ttk.Frame(root)
    content_frame.place(x=200, y=0, relwidth=1.0, relheight=1.0, anchor='nw', width=-200)
    
    canvas_projects = ttk.Canvas(content_frame, highlightthickness=0)
    scroll_y = ttk.Scrollbar(content_frame, orient="vertical", command=canvas_projects.yview)
    project_grid_frame = ttk.Frame(canvas_projects)
    project_grid_frame.bind("<Configure>", lambda e: canvas_projects.configure(scrollregion=canvas_projects.bbox("all")))
    canvas_projects.create_window((0, 0), window=project_grid_frame, anchor="nw")
    canvas_projects.configure(yscrollcommand=scroll_y.set)
    canvas_projects.pack(side="top", fill="both", expand=True, padx=20, pady=20)
    scroll_y.pack(side="right", fill="y")

    demo_logo = ImageTk.PhotoImage(Image.open("assets/Logo.png").resize((200, 200), Image.Resampling.LANCZOS))
    demo_projects = [
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
        {"title": "Test Project 1", "description": "A demo project.", "timestamp": "2025-04-29 14:30"},
        {"title": "Test Project 2", "description": "Another demo with more content and a longer description.", "timestamp": "2025-04-28 12:00"},
        {"title": "Test Project 3", "description": "Third project.", "timestamp": "2025-04-27 09:15"},
    ]
    project_tiles = []
    for proj in demo_projects:
        tile = create_project_tile(project_grid_frame, proj["title"], proj["description"], proj["timestamp"], demo_logo)
        project_tiles.append(tile)

    # --- Bottom button row ---
    bottom_controls = ttk.Frame(content_frame)
    bottom_controls.pack(side="bottom", fill="x", padx=20, pady=20)
    BU_create = ttk.Button(bottom_controls, text="Create", style="success.TButton", takefocus=False)
    BU_search = ttk.Button(bottom_controls, text="Search", style="info.TButton", takefocus=False)
    BU_create.pack(side="right", padx=(10, 10))
    BU_search.pack(side="right")

    root.bind("<Configure>", on_resize)
    root.mainloop()


# export LANG=en_US.UTF-8
# export LC_ALL=en_US.UTF-8
