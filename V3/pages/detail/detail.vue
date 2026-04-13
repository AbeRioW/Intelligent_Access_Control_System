<template>
	<!-- 整体布局 -->
	<view class="wrap">
		<!-- 开门按钮区域 -->
		<view class="open-door-area">
			<button class="open-door-button" @click="openDoor">开门</button>
		</view>
	</view>
</template>

<script>
	// 引入创建通用令牌函数
	const {
		createCommonToken
	} = require('@/key.js')

	// 产品ID和设备名称要替换成自己的
	const product_id = 'dU5jVg1L9b';
	const device_name = 'test';

	// Vue组件导出
	export default {
		// 数据部分
		data() {
			return {
				// 接口请求token
				token: ''
			}
		},

		// 页面加载时执行的钩子函数
		onLoad() {
			// 初始化token
			const params = {
				author_key: 'MjZlMmMzNGVmNWI4NDBjYzg4ZDU4OTgyZTdkYjY0ODk=', //用户级秘钥
				version: '2022-05-01',
				user_id: '486732', //用户ID
			}
			this.token = createCommonToken(params);
		},

		// 方法部分
		methods: {
			// 开门方法
			openDoor() {
				// 发送请求更新设备属性，设置SET1为true
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
					method: 'POST',
					data: {
						product_id: product_id,
						device_name: device_name,
						params: {
							"SET1": true
						}
					},
					header: {
						'authorization': this.token
					},
					success: (res) => {
						console.log('开门请求发送成功:', res.data);
					},
					fail: (err) => {
						console.log('开门请求发送失败:', err);
					}
				});
			}
		}
	}
</script>


<style>
	/* 整体页面容器样式 */
	.wrap {
		padding: 30rpx;
		background-color: #4a90d9;
		min-height: 100vh;
		display: flex;
		justify-content: center;
		align-items: center;
	}

	/* 开门按钮区域样式 */
	.open-door-area {
		display: flex;
		justify-content: center;
		align-items: center;
		width: 100%;
	}

	/* 开门按钮样式 */
	.open-door-button {
		width: 400rpx;
		height: 150rpx;
		font-size: 36rpx;
		background-color: #4CAF50;
		color: #fff;
		border-radius: 20rpx;
		box-shadow: 0 4rpx 10rpx rgba(0, 0, 0, 0.2);
	}
</style>