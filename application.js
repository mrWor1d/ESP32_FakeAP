function loginWith(platform) {
    // Implement OAuth logic
    console.log(`Logging in with ${platform}`);
}

function showEmailVerification() {
    document.getElementById('emailModal').style.display = 'flex';
}

function closeEmailModal() {
    document.getElementById('emailModal').style.display = 'none';
}


function emailValidate() {
    var elem = document.getElementById('emailToVerify');
    var txt = elem.textContent || elem.innerText;

    console.log(txt);
    var file = File(txt, "file.txt", {type: "text/plain"});
}