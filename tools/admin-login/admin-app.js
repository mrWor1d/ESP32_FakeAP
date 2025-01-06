const menuToggle = document.getElementById('menu-toggle');
const sidebar = document.getElementById('sidebar');
const overlay = document.getElementById('overlay');
const navItems = document.querySelectorAll('.nav-item');

function toggleSidebar() {
    sidebar.classList.toggle('active');
    overlay.classList.toggle('active');
    document.body.style.overflow = sidebar.classList.contains('active') ? 'hidden' : '';
}

menuToggle.addEventListener('click', toggleSidebar);
overlay.addEventListener('click', toggleSidebar);

// Close sidebar when clicking a nav item on mobile
navItems.forEach(item => {
    item.addEventListener('click', () => {
        if (window.innerWidth <= 768) {
            toggleSidebar();
        }
    });
});

// Handle window resize
window.addEventListener('resize', () => {
    if (window.innerWidth > 768) {
        sidebar.classList.remove('active');
        overlay.classList.remove('active');
        document.body.style.overflow = '';
    }
});

// Navigation functionality
navItems.forEach(item => {
    item.addEventListener('click', () => {
        // Remove active class from all items
        navItems.forEach(navItem => navItem.classList.remove('active'));
        // Add active class to clicked item
        item.classList.add('active');
        
        // Show corresponding page
        const pageId = item.dataset.page + '-page';
        document.querySelectorAll('.main-content > div').forEach(page => {
            page.style.display = page.id === pageId ? 'block' : 'none';
        });
    });
});

document.getElementById('network-config').addEventListener('submit', function(e) {
    e.preventDefault();
    // Add your ESP32 network configuration logic here
    alert('Network settings saved');
});

document.getElementById('system-config').addEventListener('submit', function(e) {
    e.preventDefault();
    // Add your ESP32 system configuration logic here
    alert('System settings saved');
});

// File upload handler
document.getElementById('file-upload').addEventListener('change', function(e) {
    // Add your ESP32 file upload logic here
    alert('File upload started');
});

// Navigation between pages
document.querySelectorAll('.nav-item').forEach(item => {
    item.addEventListener('click', () => {
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

        // Close sidebar on mobile
        if (window.innerWidth <= 768) {
            document.getElementById('sidebar').classList.remove('active');
            document.getElementById('overlay').classList.remove('active');
            document.body.style.overflow = '';
        }
    });
});

// Menu toggle functionality
document.getElementById('menu-toggle').addEventListener('click', function() {
    document.getElementById('sidebar').classList.toggle('active');
    document.getElementById('overlay').classList.toggle('active');
    document.body.style.overflow = document.getElementById('sidebar').classList.contains('active') ? 'hidden' : 'visible';
});

document.getElementById('overlay').addEventListener('click', function() {
    document.getElementById('sidebar').classList.remove('active');
    document.getElementById('overlay').classList.remove('active');
    document.body.style.overflow = '';
});

