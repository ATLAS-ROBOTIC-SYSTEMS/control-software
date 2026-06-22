const canvas = document.getElementById('joystick-canvas');
const ctx = canvas.getContext('2d');
const toggle = document.getElementById('src-toggle');
const joyWrap = document.getElementById('joy-wrap');
const badge = document.getElementById('status-badge');
const lblWeb = document.getElementById('lbl-web');
const lblCtrl = document.getElementById('lbl-ctrl');

const W = canvas.width, H = canvas.height;
const CX = W / 2, CY = H / 2;
const OUTER_R = 108, KNOB_R = 28;

let joyX = 0, joyY = 0;
let dragging = false;
let sendInterval = null;

function drawJoystick() {
    ctx.clearRect(0, 0, W, H);

    // Outer ring
    ctx.beginPath();
    ctx.arc(CX, CY, OUTER_R, 0, Math.PI * 2);
    ctx.strokeStyle = '#2c2c2a';
    ctx.lineWidth = 1.5;
    ctx.stroke();

    // Cross hairs
    ctx.strokeStyle = '#2c2c2a';
    ctx.lineWidth = 0.5;
    ctx.beginPath(); ctx.moveTo(CX, CY - OUTER_R + 8); ctx.lineTo(CX, CY + OUTER_R - 8); ctx.stroke();
    ctx.beginPath(); ctx.moveTo(CX - OUTER_R + 8, CY); ctx.lineTo(CX + OUTER_R - 8, CY); ctx.stroke();

    // Dead-zone ring
    ctx.beginPath();
    ctx.arc(CX, CY, OUTER_R * 0.08, 0, Math.PI * 2);
    ctx.strokeStyle = '#3c3489';
    ctx.lineWidth = 0.5;
    ctx.stroke();

    // Knob position
    const kx = CX + joyX * OUTER_R;
    const ky = CY - joyY * OUTER_R;

    // Line from center
    ctx.beginPath();
    ctx.moveTo(CX, CY);
    ctx.lineTo(kx, ky);
    ctx.strokeStyle = '#534ab7';
    ctx.lineWidth = 1.5;
    ctx.stroke();

    // Knob fill
    ctx.beginPath();
    ctx.arc(kx, ky, KNOB_R, 0, Math.PI * 2);
    ctx.fillStyle = dragging ? '#534ab7' : '#3c3489';
    ctx.fill();

    // Knob border
    ctx.beginPath();
    ctx.arc(kx, ky, KNOB_R, 0, Math.PI * 2);
    ctx.strokeStyle = dragging ? '#afa9ec' : '#7f77dd';
    ctx.lineWidth = 1;
    ctx.stroke();
}



function pointerToJoy(e) {
    const rect = canvas.getBoundingClientRect();
    const px = (e.clientX - rect.left) / rect.width * W;
    const py = (e.clientY - rect.top) / rect.height * H;
    let dx = (px - CX) / OUTER_R;
    let dy = -(py - CY) / OUTER_R;
    const dist = Math.sqrt(dx * dx + dy * dy);
    if (dist > 1) { dx /= dist; dy /= dist; }
    joyX = dx; joyY = dy;
}

canvas.addEventListener('pointerdown', e => {
    if (toggle.checked) return; // controller mode
    dragging = true;
    canvas.setPointerCapture(e.pointerId);
    pointerToJoy(e);
    drawJoystick();  sendJoy();
});
canvas.addEventListener('pointermove', e => {
    if (!dragging) return;
    pointerToJoy(e);
    drawJoystick(); 
});
canvas.addEventListener('pointerup', () => {
    dragging = false;
    joyX = 0; joyY = 0;
    drawJoystick(); sendJoy();
});

function sendJoy() {
    fetch(`/joy?x=${joyX.toFixed(4)}&y=${joyY.toFixed(4)}`).catch(() => { });
}


setInterval(() => {
    if (dragging && !toggle.checked) sendJoy();
}, 50);


function syncStatus() {
    fetch('/status')
        .then(r => r.json())
        .then(d => {
            if (d.useController !== toggle.checked) {
                toggle.checked = d.useController;
                applyToggle();
            }
        })
        .catch(() => { });
}
setInterval(syncStatus, 1000);

function applyToggle() {
    const useCtrl = toggle.checked;
    badge.textContent = useCtrl ? 'Controller' : 'Web';
    badge.className = 'status-badge' + (useCtrl ? ' controller' : '');
    joyWrap.classList.toggle('disabled', useCtrl);
    lblWeb.classList.toggle('active', !useCtrl);
    lblCtrl.classList.toggle('active', useCtrl);
    if (!useCtrl && !dragging) { joyX = 0; joyY = 0; drawJoystick(); }
}

toggle.addEventListener('change', () => {
    fetch(`/set_source?ctrl=${toggle.checked ? 1 : 0}`).catch(() => { });
    applyToggle();
});

applyToggle();
drawJoystick();
