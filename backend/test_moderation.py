#!/usr/bin/env python3
"""
测试OpenAI内容审核功能
"""

import asyncio
import os
from openai_service import content_moderator

async def test_moderation():
    """测试不同类型的评论内容"""
    
    test_comments = [
        ("这个英雄很强", "正常评论"),
        ("", "空评论"),
        ("垃圾游戏，垃圾开发商，都是傻子", "可能违规评论"),
        ("这英雄技能很酷，我很喜欢", "正常评论"),
        ("操你妈的垃圾", "明显违规"),
        ("不错的游戏体验", "正常评论"),
    ]
    
    print("开始测试内容审核功能...")
    print(f"OpenAI API 启用状态: {content_moderator.enabled}")
    
    for comment, description in test_comments:
        print(f"\n测试评论: '{comment}' ({description})")
        
        # 测试基础审核API
        result1 = await content_moderator.moderate_comment(comment)
        print(f"  基础审核结果: {result1}")
        
        # 测试GPT审核
        result2 = await content_moderator.moderate_comment_with_gpt(comment)
        print(f"  GPT审核结果: {result2}")

if __name__ == "__main__":
    asyncio.run(test_moderation())
