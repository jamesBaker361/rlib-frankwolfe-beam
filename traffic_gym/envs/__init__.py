from gym.envs.registration import register

register(id='TrafficEnv-v0',
    entry_point='envs.traffic_env_dir:TrafficEnv'
)

register(id='TrafficEnvLength-v0',
    entry_point='envs.traffic_env_dir:TrafficEnvLength'
)

register(id='TrafficEnvSpeed-v0',
    entry_point='envs.traffic_env_dir:TrafficEnvSpeed'
)