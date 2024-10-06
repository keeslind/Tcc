module.exports = {
  apps : [{
    name: "mqtt dashboard",
    script: 'npm',
    args: "start",
    cwd: "/home/leo/Node/mqtt-dashboard",
    watch: true,
    env: {
      NODE_ENV: "production",
      PORT: 8080,
    }
  }]
};
