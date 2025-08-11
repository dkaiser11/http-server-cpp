// Simple JavaScript for the HTTP Server demo
console.log('Static file serving is working!');

document.addEventListener('DOMContentLoaded', function () {
    // Add some interactive behavior
    const links = document.querySelectorAll('a');

    links.forEach(link => {
        link.addEventListener('click', function (e) {
            console.log('Navigating to:', this.href);
        });
    });

    // Add a timestamp to show when the page was loaded
    const container = document.querySelector('.container');
    if (container) {
        const timestamp = document.createElement('p');
        timestamp.innerHTML = '<em>Page loaded at: ' + new Date().toLocaleString() + '</em>';
        timestamp.style.fontSize = '0.9em';
        timestamp.style.color = '#999';
        timestamp.style.borderTop = '1px solid #eee';
        timestamp.style.paddingTop = '15px';
        timestamp.style.marginTop = '20px';
        container.appendChild(timestamp);
    }
});
