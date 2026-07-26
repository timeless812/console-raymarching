<br />
<div align="left">
  <h3 align="left">The Raymarhing engine written in C with rendering from console.</h3>
  <a href="https://github.com/timeless812/console-raymarching">
    <img src="pic/arch_linux_kitty.jpg" alt="screenshot" width="400" height="400">
    <img src="pic/windows(wineconsole).jpg" alt="screenshot" width="400" height="400">
  </a>
  <p align="left">
    Works simply from the console using <a href="https://en.wikipedia.org/wiki/Ncurses">ncurses</a>.
  </p>
</div>
<div align="left">
  <h2 align="left">Overview</h2>
  <p>This engine can only take the object's SDF function (write it to src/cpu_engine.c or kernel.cl, <br>depending on whether you want to run on CPU or GPU) and render it using remarching.</p>
  <p>In my engine, there is only light calculation, no shadows or reflections.</p>

  <p>The camera is controlled from the keyboard (implemented via getch() in ncurses).</p>
  <p>In the previous version, there was control over the rotation of the camera with the mouse, <br>which I implemented by reading events in /dev/input/mice, but this kind of requires sudo, so I removed that.</p>
  <h2>Camera Control</h2>
  <p>W, A, S, D - movement,<br>I, J, K, L - rotate.</p>
  <p>Also you may change the speed and sensitivity in src/camera.c.</p>
</ br>
