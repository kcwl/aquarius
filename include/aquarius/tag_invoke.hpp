#pragma once

template <typename Tag, typename... Args>
auto tag_invoke(const Tag&, Args&&...)
{
	return;
}