#!/usr/bin/env python3
"""
launch_sim.py  (ROS 2 Jazzy + Gazebo Harmonic + Nav2)

Brings up N TurtleBot3 agents in Gazebo and a full Nav2/AMCL stack per Agent:

  - Gazebo runs server-only / headless (no gzclient window)
  - Nav2 lifecycle nodes autostart
  - AMCL is auto-seeded with each Agent's known spawn pose

"""
import os
import math
from launch import LaunchDescription
from launch.actions import ExecuteProcess

# --- CONFIGURATION ---
agents = [
    {'name': 'robot1', 'x': 0.5, 'y': 0.5, 'yaw': 0.0},
    {'name': 'robot2', 'x': -0.5, 'y': -0.5, 'yaw': 1.5707},
    {'name': 'robot3', 'x': 2.0, 'y': 0.0, 'yaw': 3.1416},
]

BOOT_GRACE_PERIOD = 15.0
POLL_INTERVAL = 2.0       

def _agents_launch_arg() -> str:
    """Builds the verified CLI Agent configuration mapping string."""
    return '; '.join(
        f"{r['name']}={{x: {r['x']}, y: {r['y']}, yaw: {r['yaw']}}}"
        for r in agents
    )

def _initial_pose_yaml(x, y, yaw):
    qz = math.sin(yaw / 2.0)
    qw = math.cos(yaw / 2.0)
    cov_str = "0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0685"
    return (
        f"{{header: {{frame_id: 'map'}}, "
        f"pose: {{pose: {{position: {{x: {x}, y: {y}, z: 0.0}}, "
        f"orientation: {{z: {qz}, w: {qw}}}}}, "
        f"covariance: [{cov_str}]}}}}"
    )

def generate_launch_description():
    agents_str = _agents_launch_arg()
    
    # Path to the map file (adjust as needed)
    map_path = '/opt/ros/jazzy/share/nav2_bringup/maps/tb3_sandbox.yaml'

    launch_multi_agent = ExecuteProcess(
        cmd=[
            'ros2', 'launch', 'nav2_bringup', 'cloned_multi_tb3_simulation_launch.py',
            'headless:=False',
            'use_rviz:=True',
            'autostart:=True',
            f'map:={map_path}',
            f'robots:={agents_str}'
        ],
        output='screen'
    )

    actions = [launch_multi_agent]

    # Automated Initialization
    for r in agents:
        ns = r['name']
        pose_cmd = _initial_pose_yaml(r['x'], r['y'], r['yaw'])
        
        wait_and_publish = ExecuteProcess(
        cmd=['bash', '-c', f'''
            sleep {BOOT_GRACE_PERIOD}
            echo "[{ns}] Waiting for AMCL to activate on its own..."
            until ros2 lifecycle get /{ns}/amcl 2>/dev/null | grep -q active; do
                sleep {POLL_INTERVAL}
            done
            echo "[{ns}] AMCL is active! Publishing localized position..."
            ros2 topic pub --once /{ns}/initialpose \
                geometry_msgs/msg/PoseWithCovarianceStamped \
                "{pose_cmd}"
            echo "[{ns}] Localization complete!"
        '''],
        output='screen',
    )
        actions.append(wait_and_publish)

    return LaunchDescription(actions)