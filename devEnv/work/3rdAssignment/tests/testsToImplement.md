# Recommended Manual Tests

## 1) Happy Path: service and log in /tmp

1. `sudo systemctl restart assignment-sensor.service`
2. Wait >=2 intervals (default 5 s).
3. `tail -n 5 /tmp/assignment_sensor.log`
   - should see `YYYY-MM-DDTHH:MM:SS.mmmZ | 0xXXXXXXXX` on each line.

## 2) Fallback when /tmp is not writable

1. Run in an environment that mounts `/tmp` as non-writable (container, chroot, or `chmod 500 /tmp` temporarily inside a test container).
2. Start the service.
3. `tail -n 5 /var/tmp/assignment_sensor.log`
   - should see new lines.
4. `journalctl -u assignment-sensor.service --since -5m | grep fallback`
   - A warning indicating the use of `/var/tmp` should appear.

## 3) Clean Termination with SIGTERM

1. `sudo systemctl start assignment-sensor.service`
2. `sudo systemctl stop assignment-sensor.service`
3. `systemctl status assignment-sensor.service`
   - Status `inactive (dead)` without unexpected restarts.
4. Check the log: there should be no truncated lines.

## 4) Error due to non-existent device

1. Edit the unit to use `--device /dev/fake0`.
2. `sudo systemctl daemon-reload && sudo systemctl restart assignment-sensor.service`
3. `systemctl status assignment-sensor.service`
   - It should show immediate failure and a non-zero exit code.

## 5) Automatic Restart (optional)

1. With the service active, kill the process: `sudo pkill assignment-sensor`
2. `systemctl status assignment-sensor.service`
   - It should reactivate due to `Restart=on-failure` (observe `Active: active (running)` after a moment).

## 6) Interval Adjustment

1. Edit the unit to use `--interval 1`.
2. Restart the service.
3. Verify that the log grows approximately one line per second.
