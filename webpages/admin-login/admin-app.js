const menuToggle = document.getElementById("menu-toggle");
const sidebar    = document.getElementById("sidebar");
const overlay    = document.getElementById("overlay");
const navItems   = document.querySelectorAll(".nav-item");

function toggleSidebar() {
    sidebar.classList.toggle("active");
    overlay.classList.toggle("active");
    document.body.style.overflow = sidebar.classList.contains("active") ? "hidden" : "";
}

menuToggle.addEventListener("click", toggleSidebar);
overlay.addEventListener("click", toggleSidebar);

// Close sidebar when clicking a nav item on mobile
navItems.forEach((item) => {
    item.addEventListener("click", () => {
         // Remove active class from all items
         document.querySelectorAll('.nav-item').forEach(navItem => {
            navItem.classList.remove('active');
        });
        
        // Add active class to clicked item
        item.classList.add('active');
        
        // Hide all pages
        document.querySelectorAll('.main-content > div').forEach(page => {
            page.style.display = 'none';
        });
        
        // Show selected page
        const pageId = item.getAttribute('data-page') + '-page';
        document.getElementById(pageId).style.display = 'block';

        if (window.innerWidth <= 768) {
            toggleSidebar();
        }
    });
});

// Handle window resize
window.addEventListener("resize", () => {
    if (window.innerWidth > 768) {
        sidebar.classList.remove("active");
        overlay.classList.remove("active");
        document.body.style.overflow = "";
    }
});



// Initialize file management

const uploadArea = document.getElementById('upload-area');
const fileInput  = document.getElementById('file-upload');


// Drag and drop handlers
['dragenter', 'dragover', 'dragleave', 'drop'].forEach(eventName => {
    uploadArea.addEventListener(eventName, preventDefaults, false);
});

function preventDefaults(e) {
    e.preventDefault();
    e.stopPropagation();
}

['dragenter', 'dragover'].forEach(eventName => {
    uploadArea.addEventListener(eventName, highlight, false);
});

['dragleave', 'drop'].forEach(eventName => {
    uploadArea.addEventListener(eventName, unhighlight, false);
});

function highlight(e) {
    uploadArea.classList.add('drag-over');
}

function unhighlight(e) {
    uploadArea.classList.remove('drag-over');
}

uploadArea.addEventListener('drop', handleDrop, false);

function handleDrop(e) {
    const dt = e.dataTransfer;
    const files = dt.files;
    handleFiles(files);
}

fileInput.addEventListener('change', function(e) {
    handleFiles(this.files);
});

function handleFiles(files) {
    // Here you would implement your file upload logic
    fetch("/admin-panel/updload", {
        method: 'POST',
        headers: {
            'Content-Type': 'multipart/form-data',
        }
    });
    console.log('Files to upload:', files);
    // You can send these files to your ESP32 server
}

/*
const ITEMS_PER_PAGE = 10; // Number of files to show per page
let currentFolderData = {};

async function loadFileData() {
    try {
        const response = await fetch("admin-panel/data.json");
        const data = await response.json();
        console.log(data);
        renderFolders(data.folders);
    } catch (error) {
        console.error("Error loading file data:", error);
    }
}

function renderFolders(folders) {
    const container = document.getElementById("folders-container");
    container.innerHTML = "";

    folders.forEach((folder) => {
        const folderElement = createFolderElement(folder);
        container.appendChild(folderElement);
    });
}

function createFolderElement(folder) {
    const folderSection = document.createElement("div");
    folderSection.className = "folder-section";

    // Create folder header
    const header = document.createElement("div");
    header.className = "folder-header";
    header.innerHTML = `
                <h3>
                    <span>📁</span>
                    ${folder.name}
                    <span style="color: #666; font-size: 0.9rem;">(${folder.files.size} files)</span>
                </h3>
                <span class="folder-toggle">▼</span>
            `;

    // Create folder content
    const content = document.createElement("div");
    content.className = "folder-content";

    // Store folder data for pagination
    currentFolderData[folder.name] = {
        files: folder.files,
        currentPage: 1,
    };

    // Render initial page
    renderFolderContent(content, folder.name);

    // Add click handler for folder toggle
    header.addEventListener("click", () => {
        content.classList.toggle("active");
        header.querySelector(".folder-toggle").textContent =
            content.classList.contains("active") ? "▼" : "▶";
    });

    folderSection.appendChild(header);
    folderSection.appendChild(content);
    return folderSection;
}

function renderFolderContent(container, folderName) {
    const folderData = currentFolderData[folderName];
    const { files, currentPage } = folderData;

    // Calculate pagination
    const totalPages = Math.ceil(files.length / ITEMS_PER_PAGE);
    const startIndex = (currentPage - 1) * ITEMS_PER_PAGE;
    const endIndex = startIndex + ITEMS_PER_PAGE;
    const currentFiles = files.slice(startIndex, endIndex);

    // Create content
    let html = '<div class="file-list">';

    if (currentFiles.length === 0) {
        html += '<div class="folder-empty">No files in this folder</div>';
    } else {
        currentFiles.forEach((file) => {
            html += `
                        <div class="file-item">
                            <div class="file-info">
                                <span class="file-name">${file.name}</span>
                                <span class="file-meta">${file.size}</span>
                                <span class="file-meta">${file.path}</span>
                            </div>
                            <div class="file-actions">
                                <button class="btn btn-small btn-primary" onclick="downloadFile('${folderName}', '${file.name}')">Download</button>
                                <button class="btn btn-small btn-primary" onclick="editFile('${folderName}', '${file.name}')">Edit</button>
                                <button class="btn btn-small btn-primary" onclick="deleteFile('${folderName}', '${file.name}')">Delete</button>
                            </div>
                        </div>
                    `;
        });
    }

    // Add pagination if needed
    if (totalPages > 1) {
        html += '<div class="pagination">';
        // Previous button
        html += `
                    <button class="page-btn" 
                            ${currentPage === 1 ? "disabled" : ""} 
                            onclick="changePage('${folderName}', ${currentPage - 1})">
                        Previous
                    </button>
                `;
        // Page numbers
        for (let i = 1; i <= totalPages; i++) {
            html += `
                        <button class="page-btn ${currentPage === i ? "active" : ""}" 
                                onclick="changePage('${folderName}', ${i})">
                            ${i}
                        </button>
                    `;
        }
        // Next button
        html += `
                    <button class="page-btn" 
                            ${currentPage === totalPages ? "disabled" : ""} 
                            onclick="changePage('${folderName}', ${currentPage + 1})">
                        Next
                    </button>
                `;
        html += "</div>";
    }

    html += "</div>";
    container.innerHTML = html;
}

function changePage(folderName, newPage) {
    const folderData = currentFolderData[folderName];
    const totalPages = Math.ceil(folderData.files.length / ITEMS_PER_PAGE);

    if (newPage >= 1 && newPage <= totalPages) {
        folderData.currentPage = newPage;
        const folderElement = document.querySelector(
            `[data-folder="${folderName}"]`
        );
        renderFolderContent(
            folderElement.querySelector(".folder-content"),
            folderName
        );
    }
}
*/
